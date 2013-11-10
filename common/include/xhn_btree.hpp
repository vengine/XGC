//
//  xhn_btree.h
//  7z
//
//  Created by 徐 海宁 on 13-11-6.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#ifndef ___z__xhn_btree__
#define ___z__xhn_btree__

#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "emem.hpp"
#include <new>

#include "xhn_config.hpp"

namespace xhn
{
    template <typename Addr, typename RefAddr>
    class btree_node
    {
    public:
        volatile esint32 ref_count;
        euint8 section;
		btree_node* parent;
        btree_node* children[16];
        euint32 num_children;
        Addr begin_addr;
        Addr end_addr;
        bool is_deleted;
    public:
        btree_node()
        : ref_count(0)
        , section(0)
		, parent(NULL)
        , num_children(0)
        , begin_addr( (Addr)((RefAddr)-1) )
        , end_addr( NULL )
        , is_deleted( false )
        {
            memset(children, 0, sizeof(children));
        }
        btree_node* get_child_node(euint8 sec) {
            return children[sec & 0x0f];
        }
        btree_node* get_child_node(Addr ptr) {
            if ((RefAddr)ptr >= (RefAddr)begin_addr &&
                (RefAddr)ptr <= (RefAddr)end_addr) {
                for (int i = 0; i < 16; i++) {
                    btree_node* node = children[i];
                    if (!node)
                        continue;
                    if ((RefAddr)ptr >= (RefAddr)node->begin_addr &&
                        (RefAddr)ptr <= (RefAddr)node->end_addr) {
                        return node;
                    }
                }
            }
            return NULL;
        }
        bool add_child_node(euint8 sec, btree_node* node) {
            if (children[sec & 0x0f])
                return false;
            children[sec & 0x0f] = node;
            node->inc_ref_count();
			node->parent = this;
            num_children++;
            return true;
        }
        
        btree_node* remove_child_node(euint8 sec, bool& is_deleted) {
            btree_node* node = children[sec & 0x0f];
            if (node) {
                children[sec & 0x0f] = NULL;
                num_children--;
                if (!num_children)
                    is_deleted = true;
                else
                    is_deleted = false;
                node->dec_ref_count();
                return node;
            }
            else {
                is_deleted = false;
                return 0;
            }
        }
        
        void add_update(btree_node* node) {
            if ((RefAddr)node->begin_addr < (RefAddr)begin_addr)
                begin_addr = node->begin_addr;
            if ((RefAddr)node->end_addr > (RefAddr)end_addr)
                end_addr = node->end_addr;
        }
        void remove_update() {
            begin_addr = (Addr)((RefAddr)-1);
            end_addr = 0;
            for (int i = 0; i < 16; i++) {
                btree_node* node = children[i];
                if (!node)
                    continue;
                if ((RefAddr)node->begin_addr < (RefAddr)begin_addr)
                    begin_addr = node->begin_addr;
                if ((RefAddr)node->end_addr > (RefAddr)end_addr)
                    end_addr = node->end_addr;
            }
        }
        inline void inc_ref_count() {
            AtomicIncrement(&ref_count);
        }
        inline esint32 dec_ref_count() {
            return AtomicDecrement(&ref_count);
        }
        inline esint32 get_ref_count() {
            return ref_count;
        }
    };
    
    template <typename T, typename Addr, typename RefAddr, typename NodeAllocator>
    class btree : public MemObject
    {
    public:
        class node_handle
        {
        public:
            T* ptr;
            inline void _inc(T* p) {
                if (p)
                {
                    p->inc_ref_count();
                }
            }
            inline void _dec() {
                if (ptr && !ptr->dec_ref_count()) {
                    NodeAllocator allocater;
                    allocater.destroy(ptr);
                    allocater.deallocate(ptr, 1);
                }
            }
            explicit node_handle(const node_handle& p)
            {
                _inc((T*)p.ptr);
                ptr = (T*)p.ptr;
            }
            explicit node_handle()
            : ptr(NULL)
            {}
            
            ~node_handle()
            {
                _dec();
            }
            void operator = (T* p)
            {
                _inc(p);
                _dec();
                ptr = p;
            }
            void operator = (node_handle& p)
            {
                _inc(p.ptr);
                _dec();
                ptr = p.ptr;
            }
            bool operator!() const {
                return !ptr;
            }
            operator bool () const {
                return ptr != NULL;
            }
            T* get() {
                return ptr;
            }
            T* operator ->() {
                return ptr;
            }
            const T* operator ->() const {
                return ptr;
            }
        };
    public:
        T* root;
		euint count;
		euint alloced_size;
        NodeAllocator allocater;
    public:
        btree()
        : count(0)
        , alloced_size(0)
        {
            root = allocater.allocate(1);
            allocater.construct(root, NULL);
            root->inc_ref_count();
        }
        T* insert(const Addr ptr, euint size, vptr data) {
			bool added = false;
            node_handle track_buffer[sizeof(ptr) * 2 + 1];
            node_handle node;
            node = root;
            euint num_bytes = sizeof(ptr);
            RefAddr mask = (RefAddr)0x0f;
            euint shift = sizeof(ptr) * 8 - 4;
            mask <<= shift;
            for (euint i = 0; i < num_bytes * 2; i++) {
                track_buffer[sizeof(ptr) * 2 - i] = node;
                RefAddr rptr = (RefAddr)ptr;
                rptr &= mask;
                rptr >>= shift;
                euint8 section = (euint8)rptr;
                node_handle next;
                next = (T*)node->get_child_node(section);
                if (!next) {
                    T* tmp = allocater.allocate(1);
                    allocater.construct(tmp, data);
                    next = tmp;
                    next->section = section;
                    node->add_child_node(section, next.get());
					added = true;
                }
                node = next;
                shift -= 4;
                mask >>= 4;
            }
            track_buffer[0] = node;
            node->begin_addr = ptr;
            node->end_addr = (Addr)((RefAddr)ptr + size - 1);
            for (euint i = 1; i < num_bytes * 2 + 1; i++) {
                track_buffer[i]->add_update(track_buffer[i - 1].get());
            }
			if (added) {
				count++;
				alloced_size += size;
                return node.get();
			}
            else {
                return NULL;
            }
        }

        bool remove(const Addr ptr) {
            node_handle track_buffer[sizeof(ptr) * 2 + 1];
            node_handle node;
            node = root;
            euint num_bytes = sizeof(ptr);
            for (euint i = 0; i < num_bytes * 2; i++) {
                track_buffer[sizeof(ptr) * 2 - i] = node;
                node_handle next;
                next = (T*)node->get_child_node(ptr);
                if (!next)
                    return false;
                node = next;
            }
			///if (node->prev) { node->prev->next = node->next; }
			///if (node->next) { node->next->prev = node->prev; }
            track_buffer[0] = node;
            
			if (node->is_deleted)
				return false;
			node->is_deleted = true;
            ///if (node->dest) {
			///	node->dest(node->begin_addr);
			///}
            allocater.pre_destroy(node.get());
			alloced_size -= ((RefAddr)node->end_addr - (RefAddr)node->begin_addr);
            
            for (euint i = 1; i < num_bytes * 2 + 1; i++) {
                node_handle next;
                node_handle prev;
                next = track_buffer[i];
                prev = track_buffer[i - 1];
                bool is_deleted = false;
                node = (T*)next->remove_child_node(prev->section, is_deleted);
                track_buffer[i - 1] = NULL;
                
                if (!is_deleted)
                    break;
            }
            for (euint i = 1; i < num_bytes * 2 + 1; i++) {
                node = track_buffer[i];
                if (node) {
                    node->remove_update();
                }
            }
			count--;
            return true;
        }
		
		bool remove(T* node) {
			if (node->is_deleted)
				return false;
			node->is_deleted = true;
            allocater.pre_destroy(node);
			alloced_size -= ((RefAddr)node->end_addr - (RefAddr)node->begin_addr);
            
            while (node->parent) {
                node_handle next = node->parent;
                bool is_deleted = false;
                next->remove_child_node(node->section, is_deleted);
                ///delete node;
                ///allocater.destroy(node);
                ///allocater.deallocate(node);
                if (!is_deleted)
                    break;
                node->remove_update();
                node = next;
            }
			count--;
            return true;
		}
		
        T* find(const Addr ptr) {
            T* node = root;
            euint num_bytes = sizeof(ptr);
            for (euint i = 0; i < num_bytes * 2; i++) {
                T* next = (T*)node->get_child_node(ptr);
                if (!next)
                    return NULL;
                node = next;
            }
            return node;
        }
		euint size() {
			return count;
		}
		euint get_alloced_size() {
			return alloced_size;
		}
    };
}


#endif /* defined(___z__xhn_btree__) */
