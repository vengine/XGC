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
#include "xhn_pair.hpp"

namespace xhn
{
    template <typename Addr>
    struct range
    {
        Addr begin_addr;
        Addr end_addr;
        bool operator < (const range& a) const {
            if (end_addr && a.end_addr) {
                return begin_addr < a.begin_addr;
            }
            else if (!end_addr && a.end_addr) {
                return begin_addr < a.end_addr;
            }
            else if (end_addr && !a.end_addr) {
                return end_addr < a.begin_addr;
            }
            else
                return false;
        }
    };
    template <typename Addr, typename RefAddr, typename SetNodeAllocator>
    class btree_node
    {
    public:
        euint8 section;
		btree_node* parent;
        btree_node* children[16];
        euint32 num_children;
        Addr begin_addr;
        Addr end_addr;
        bool is_deleted;
    public:
        btree_node()
        : section(0)
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
    };
    
    template <typename T, typename Addr, typename RefAddr, typename NodeAllocator>
    class btree : public MemObject
    {
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
        }
        T* insert(const Addr ptr, euint size, vptr data) {
			bool added = false;
            T* track_buffer[sizeof(ptr) * 2 + 1];
            T* node = root;
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
                T* next = (T*)node->get_child_node(section);
                if (!next) {
                    next = allocater.allocate(1);
                    allocater.construct(next, data);
                    next->section = section;
                    node->add_child_node(section, next);
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

        bool remove(const Addr ptr) {
            T* track_buffer[sizeof(ptr) * 2 + 1];
            T* node = root;
            euint num_bytes = sizeof(ptr);
            for (euint i = 0; i < num_bytes * 2; i++) {
                track_buffer[sizeof(ptr) * 2 - i] = node;
                T* next = (T*)node->get_child_node(ptr);
                if (!next)
                    return false;
                node = next;
            }
            track_buffer[0] = node;
            
			if (node->is_deleted)
				return false;
			node->is_deleted = true;
            allocater.pre_destroy(node);
			alloced_size -= ((RefAddr)node->end_addr - (RefAddr)node->begin_addr);
            
            for (euint i = 1; i < num_bytes * 2 + 1; i++) {
                T* next = track_buffer[i];
                T* prev = track_buffer[i - 1];
                bool is_deleted = false;
                node = (T*)next->remove_child_node(prev->section, is_deleted);
                allocater.destroy(node);
                allocater.deallocate(node, 1);
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
                T* next = node->parent;
                bool is_deleted = false;
                next->remove_child_node(node->section, is_deleted);
                allocater.destroy(node);
                allocater.deallocate(node, 1);
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
    
    extern Unlocked_mem_allocator* s_unlocked_allocator;
    template <typename K, typename V>
    class UnlockedMapNodeAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef rbtree_node<K>* pointer;
        typedef const rbtree_node<K>* const_pointer;
        typedef rbtree_node<K> value_type;
        typedef rbtree_node<K>& reference;
        typedef const rbtree_node<K>& const_reference;
        
        template<typename AK, typename AV>
        struct rebind
        {
            typedef UnlockedMapNodeAllocator<AK, AV> other;
        };
        
        pointer address(reference v) const
        {
            return &v;
        }
        
        const_pointer address(const_reference v) const
        {
            return &v;
        }
        
        UnlockedMapNodeAllocator()
        {
        }
        
        void deallocate(pointer ptr, size_type)
        {
            UnlockedMemAllocator_free(s_unlocked_allocator, ptr);
        }
        
        pointer allocate(size_type count)
        {
            if (!s_unlocked_allocator) {
                s_unlocked_allocator = UnlockedMemAllocator_new();
            }
            return (pointer)UnlockedMemAllocator_alloc(
                                                       s_unlocked_allocator,
                                                       count * sizeof(value_type),
                                                       false);
        }
        
        pointer allocate(size_type count, const void*)
        {
            if (!s_unlocked_allocator) {
                s_unlocked_allocator = UnlockedMemAllocator_new();
            }
            return (pointer)UnlockedMemAllocator_alloc(
                                                       s_unlocked_allocator,
                                                       count * sizeof(value_type),
                                                       false);
        }
        
        void construct(pointer ptr, const K& v)
        {
            new (ptr) pair<K, V>();
        }
        
        void construct(pointer ptr)
        {
            new ( ptr ) pair<K, V> ();
        }
        
        void destroy(pointer ptr)
        {
            ((pair<K, V>*)ptr)->~pair<K, V>();
        }
        
        size_type max_size() const {
            return static_cast<size_type>(-1) / sizeof(value_type);
        }
    };
    
    template <typename K>
    class UnlockedSetNodeAllocator
    {
    public:
        typedef euint size_type;
        typedef euint difference_type;
        typedef rbtree_node<K>* pointer;
        typedef const rbtree_node<K>* const_pointer;
        typedef rbtree_node<K> value_type;
        typedef rbtree_node<K>& reference;
        typedef const rbtree_node<K>& const_reference;
        
        template<typename AK>
        struct rebind
        {
            typedef UnlockedSetNodeAllocator<AK> other;
        };
        
        pointer address(reference v) const
        {
            return &v;
        }
        
        const_pointer address(const_reference v) const
        {
            return &v;
        }
        
        UnlockedSetNodeAllocator()
        {
        }
        
        void deallocate(pointer ptr, size_type)
        {
            UnlockedMemAllocator_free(s_unlocked_allocator, ptr);
        }
        
        pointer allocate(size_type count)
        {
            if (!s_unlocked_allocator) {
                s_unlocked_allocator = UnlockedMemAllocator_new();
            }
            return (pointer)UnlockedMemAllocator_alloc(
                                                       s_unlocked_allocator,
                                                       count * sizeof(value_type),
                                                       false);
        }
        
        pointer allocate(size_type count, const void*)
        {
            if (!s_unlocked_allocator) {
                s_unlocked_allocator = UnlockedMemAllocator_new();
            }
            return (pointer)UnlockedMemAllocator_alloc(
                                                       s_unlocked_allocator,
                                                       count * sizeof(value_type),
                                                       false);
        }
        
        void construct(pointer ptr, const K& v)
        {
            new (ptr) rbtree_node<K>();
        }
        
        void construct(pointer ptr)
        {
            new ( ptr ) rbtree_node<K> ();
        }
        
        void destroy(pointer ptr)
        {
            ptr->~rbtree_node<K>();
        }
        
        size_type max_size() const {
            return static_cast<size_type>(-1) / sizeof(value_type);
        }
    };
}


#endif /* defined(___z__xhn_btree__) */
