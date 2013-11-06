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
    class btree_node
    {
    public:
        euint8 section;
		btree_node* parent;
        btree_node* children[16];
        euint32 num_children;
        vptr begin_addr;
        vptr end_addr;
        bool is_deleted;
    public:
        btree_node()
        : section(0)
		, parent(NULL)
        , num_children(0)
        , begin_addr( (vptr)((ref_ptr)-1) )
        , end_addr( NULL )
        , is_deleted( false )
        {
            memset(children, 0, sizeof(children));
        }
        btree_node* get_child_node(euint8 sec) {
            return children[sec & 0x0f];
        }
        btree_node* get_child_node(vptr ptr) {
            if ((ref_ptr)ptr >= (ref_ptr)begin_addr &&
                (ref_ptr)ptr <= (ref_ptr)end_addr) {
                for (int i = 0; i < 16; i++) {
                    btree_node* node = children[i];
                    if (!node)
                        continue;
                    if ((ref_ptr)ptr >= (ref_ptr)node->begin_addr &&
                        (ref_ptr)ptr <= (ref_ptr)node->end_addr) {
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
                return node;
            }
            else {
                is_deleted = false;
                return 0;
            }
        }
        
        void add_update(btree_node* node) {
            if ((ref_ptr)node->begin_addr < (ref_ptr)begin_addr)
                begin_addr = node->begin_addr;
            if ((ref_ptr)node->end_addr > (ref_ptr)end_addr)
                end_addr = node->end_addr;
        }
        void remove_update() {
            begin_addr = (vptr)((ref_ptr)-1);
            end_addr = 0;
            for (int i = 0; i < 16; i++) {
                btree_node* node = children[i];
                if (!node)
                    continue;
                if ((ref_ptr)node->begin_addr < (ref_ptr)begin_addr)
                    begin_addr = node->begin_addr;
                if ((ref_ptr)node->end_addr > (ref_ptr)end_addr)
                    end_addr = node->end_addr;
            }
        }
    };
    
    template <typename NodeAllocator>
    class btree : public MemObject
    {
    public:
        btree_node* root;
		euint count;
		euint alloced_size;
        NodeAllocator allocater;
    public:
        btree()
        : count(0)
        , alloced_size(0)
        {
            root = allocater.allocate(1);
            allocater.construct(root);
        }
        btree_node* insert(const vptr ptr, euint size) {
			bool added = false;
            btree_node* track_buffer[sizeof(ptr) * 2 + 1];
            btree_node* node = root;
            euint num_bytes = sizeof(ptr);
            ref_ptr mask = (ref_ptr)0x0f;
            euint shift = sizeof(ptr) * 8 - 4;
            mask <<= shift;
            for (euint i = 0; i < num_bytes * 2; i++) {
                track_buffer[sizeof(ptr) * 2 - i] = node;
                ref_ptr rptr = (ref_ptr)ptr;
                rptr &= mask;
                rptr >>= shift;
                euint8 section = (euint8)rptr;
                btree_node* next = node->get_child_node(section);
                if (!next) {
                    next = allocater.allocate(1);
                    allocater.construct(next);
                    next->section = section;
                    node->add_child_node(section, next);
					added = true;
                }
                node = next;
                shift -= 4;
                mask >>= 4;
            }
            track_buffer[0] = node;
            ///node->name = name;
            ///node->dest = dest;
            node->begin_addr = ptr;
            node->end_addr = (vptr)((ref_ptr)ptr + size);
            for (euint i = 1; i < num_bytes * 2 + 1; i++) {
                track_buffer[i]->add_update(track_buffer[i - 1]);
            }
			if (added) {
				count++;
				alloced_size += size;
                return node;
			}
            else {
                return NULL;
            }
        }

        bool remove(const vptr ptr) {
            btree_node* track_buffer[sizeof(ptr) * 2 + 1];
            btree_node* node = root;
            euint num_bytes = sizeof(ptr);
            for (euint i = 0; i < num_bytes * 2; i++) {
                track_buffer[sizeof(ptr) * 2 - i] = node;
                btree_node* next = node->get_child_node(ptr);
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
            allocater.pre_destroy(node);
			alloced_size -= ((ref_ptr)node->end_addr - (ref_ptr)node->begin_addr);
            
            for (euint i = 1; i < num_bytes * 2 + 1; i++) {
                btree_node* next = track_buffer[i];
                btree_node* prev = track_buffer[i - 1];
                bool is_deleted = false;
                node = next->remove_child_node(prev->section, is_deleted);
                track_buffer[i - 1] = NULL;
                ///delete node;
                allocater.destroy(node);
                allocater.deallocate(node, 1);
                
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
		
		bool remove(btree_node* node) {
			if (node->is_deleted)
				return false;
			node->is_deleted = true;
            allocater.pre_destroy(node);
			alloced_size -= ((ref_ptr)node->end_addr - (ref_ptr)node->begin_addr);
            
            while (node->parent) {
                btree_node* next = node->parent;
                bool is_deleted = false;
                next->remove_child_node(node->section, is_deleted);
                ///delete node;
                allocater.destroy(node);
                allocater.deallocate(node);
                if (!is_deleted)
                    break;
                node->remove_update();
                node = next;
            }
			count--;
            return true;
		}
		
        btree_node* find(const vptr ptr) {
            btree_node* node = root;
            euint num_bytes = sizeof(ptr);
            for (euint i = 0; i < num_bytes * 2; i++) {
                btree_node* next = node->get_child_node(ptr);
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
