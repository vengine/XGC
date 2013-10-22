/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_MEM_MAP_HPP
#define XHN_MEM_MAP_HPP

#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "emem.hpp"
#include <new>

#include "xhn_iterator.hpp"
#include "xhn_utility.hpp"
#include "xhn_rbtree.hpp"
#include "xhn_pair.hpp"

#include "xhn_map.hpp"
#include "xhn_set.hpp"
#include "xhn_lock.hpp"
#include "xhn_atomic_operation.hpp"
#include "rwbuffer.h"

#include "xhn_config.hpp"

namespace xhn
{
    typedef void (*destructor) (void*);
    class mem_btree_node
    {
	public:
		//////////////////////////////////////////////////////////////////////////
		void* operator new( size_t nSize )
		{
			if (!s_allocator) {
				s_allocator = UnlockedMemAllocator_new();
			}
			return UnlockedMemAllocator_alloc(s_allocator, nSize, false);
		}
		void operator delete( void *p)
		{
			UnlockedMemAllocator_free(s_allocator, p);
		}
		void* operator new( size_t nSize, void* ptr )
		{
			EAssert(((ref_ptr)ptr % 16) == 0, "new object error");
			return ptr;
		}
		void operator delete( void *p, void* ptr )
		{
		}
		//////////////////////////////////////////////////////////////////////////
		void* operator new( size_t nSize, const char* file,int line )
		{
			if (!s_allocator) {
				s_allocator = UnlockedMemAllocator_new();
			}
			return UnlockedMemAllocator_alloc(s_allocator, nSize, false);
		}
		void operator delete( void *p, const char* file,int line )
		{
			UnlockedMemAllocator_free(s_allocator, p);
		}
		//////////////////////////////////////////////////////////////////////////
		void* operator new[]( size_t nSize )
		{
			if (!s_allocator) {
				s_allocator = UnlockedMemAllocator_new();
			}
			return UnlockedMemAllocator_alloc(s_allocator, nSize, false);
		}
		void operator delete[]( void* ptr, size_t nSize )
		{
			UnlockedMemAllocator_free(s_allocator, ptr);
		}
	public:
		static Unlocked_mem_allocator* s_allocator;
	public:
		template <typename K, typename V>
		class FMapNodeAllocator
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
				typedef FMapNodeAllocator<AK, AV> other;
			};

			pointer address(reference v) const
			{	
				return &v;
			}

			const_pointer address(const_reference v) const
			{	
				return &v;
			}

			FMapNodeAllocator()
			{
			}

			FMapNodeAllocator(const FMapNodeAllocator& rth)
			{
			}

			template<typename AK, typename AV>
			FMapNodeAllocator(const FMapNodeAllocator<AK, AV>&)
			{
			}

			template<typename AK, typename AV>
			FMapNodeAllocator<AK, AV>& operator=(const FMapNodeAllocator<AK, AV>&)
			{
				return (*this);
			}

			void deallocate(pointer ptr, size_type)
			{	
				UnlockedMemAllocator_free(mem_btree_node::s_allocator, ptr);
			}

			pointer allocate(size_type count)
			{
				if (!mem_btree_node::s_allocator) {
					mem_btree_node::s_allocator = UnlockedMemAllocator_new();
				}
				return (pointer)UnlockedMemAllocator_alloc(
					mem_btree_node::s_allocator, 
					count * sizeof(value_type), 
					false);
			}

			pointer allocate(size_type count, const void*)
			{
				if (!mem_btree_node::s_allocator) {
					mem_btree_node::s_allocator = UnlockedMemAllocator_new();
				}
				return (pointer)UnlockedMemAllocator_alloc(
					mem_btree_node::s_allocator, 
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
		class FSetNodeAllocator
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
				typedef FSetNodeAllocator<AK> other;
			};

			pointer address(reference v) const
			{	
				return &v;
			}

			const_pointer address(const_reference v) const
			{	
				return &v;
			}

			FSetNodeAllocator()
			{
			}

			FSetNodeAllocator(const FSetNodeAllocator& rth)
			{
			}

			template<typename AK>
			FSetNodeAllocator(const FSetNodeAllocator<AK>&)
			{
			}

			template<typename AK>
			FSetNodeAllocator<AK>& operator=(const FSetNodeAllocator<AK>&)
			{
				return (*this);
			}

			void deallocate(pointer ptr, size_type)
			{	
				UnlockedMemAllocator_free(mem_btree_node::s_allocator, ptr);
			}

			pointer allocate(size_type count)
			{
				if (!mem_btree_node::s_allocator) {
					mem_btree_node::s_allocator = UnlockedMemAllocator_new();
				}
				return (pointer)UnlockedMemAllocator_alloc(
					mem_btree_node::s_allocator, 
					count * sizeof(value_type), 
					false);
			}

			pointer allocate(size_type count, const void*)
			{
				if (!mem_btree_node::s_allocator) {
					mem_btree_node::s_allocator = UnlockedMemAllocator_new();
				}
				return (pointer)UnlockedMemAllocator_alloc(
					mem_btree_node::s_allocator, 
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
		struct input_pair
		{
			mem_btree_node* node;
			vptr handle;
			bool operator < (const input_pair& ip) const {
				if ((ref_ptr)node < (ref_ptr)ip.node)
					return true;
				else if ((ref_ptr)node > (ref_ptr)ip.node)
					return false;
				else {
					if ((ref_ptr)handle < (ref_ptr)ip.handle)
						return true;
					else
						return false;
				}
			}
		};
		typedef set<input_pair, 
			        FLessProc<input_pair>, 
					mem_btree_node::FSetNodeAllocator<input_pair>> input_mem_set;
        typedef map<vptr, 
			        mem_btree_node*,
		            FLessProc<vptr>,
					mem_btree_node::FMapNodeAllocator<vptr, mem_btree_node*>> mem_map;
		typedef set<mem_btree_node*,
			        FLessProc<mem_btree_node*>, 
			        mem_btree_node::FSetNodeAllocator<mem_btree_node*>> mem_set;
    public:
        euint8 section;
		mem_btree_node* parent;
        mem_btree_node* children[16];
        euint32 num_children;
        vptr begin_addr;
        vptr end_addr;
		input_mem_set input_set;
		mem_map output_map;
		esint32 root_ref_count;
		mem_btree_node* prev;
		mem_btree_node* next;
		mem_btree_node* root_prev;
		mem_btree_node* root_next;
        const char* name;
        esint32 orphan_count;
        destructor dest;
		bool is_garbage;
		bool is_deleted;
    public:
        mem_btree_node()
        : section(0)
		, parent(NULL)
        , num_children(0)
        , begin_addr( (vptr)((ref_ptr)-1) )
        , end_addr( NULL )
		, root_ref_count(0)
		, prev( NULL )
		, next( NULL )
		, root_prev( NULL )
		, root_next( NULL )
        , name( NULL )
        , orphan_count( 100 )
        , dest( NULL )
		, is_garbage(false)
		, is_deleted(false)
        {
            memset(children, 0, sizeof(children));
        }
        mem_btree_node* get_child_node(euint8 sec) {
            return children[sec & 0x0f];
        }
        mem_btree_node* get_child_node(vptr ptr) {
            if ((ref_ptr)ptr >= (ref_ptr)begin_addr &&
                (ref_ptr)ptr <= (ref_ptr)end_addr) {
                for (int i = 0; i < 16; i++) {
                    mem_btree_node* node = children[i];
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
        bool add_child_node(euint8 sec, mem_btree_node* node) {
            if (children[sec & 0x0f])
                return false;
            children[sec & 0x0f] = node;
			node->parent = this;
            num_children++;
            return true;
        }
        
        mem_btree_node* remove_child_node(euint8 sec, bool& is_deleted) {
            mem_btree_node* node = children[sec & 0x0f];
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
        
        void add_update(mem_btree_node* node) {
            if ((ref_ptr)node->begin_addr < (ref_ptr)begin_addr)
                begin_addr = node->begin_addr;
            if ((ref_ptr)node->end_addr > (ref_ptr)end_addr)
                end_addr = node->end_addr;
        }
        void remove_update() {
            begin_addr = (vptr)((ref_ptr)-1);
            end_addr = 0;
            for (int i = 0; i < 16; i++) {
                mem_btree_node* node = children[i];
                if (!node)
                    continue;
                if ((ref_ptr)node->begin_addr < (ref_ptr)begin_addr)
                    begin_addr = node->begin_addr;
                if ((ref_ptr)node->end_addr > (ref_ptr)end_addr)
                    end_addr = node->end_addr;
            }
        }

		void Attach(const vptr handle, mem_btree_node* mem);
		void Detach(const vptr handle, mem_btree_node* mem);
		void AttchToRoot();
		void DetachFromRoot();
		void MarkNotGarbage();
        bool _TrackBack(mem_set& trackBuffer);
        void _Erase();
        bool TrackBack();
    };
    
    class mem_btree
    {
    public:
        mem_btree_node* root;
		euint count;
		euint alloced_size;
    public:
        mem_btree()
			: count(0)
			, alloced_size(0)
        {
            root = ENEW mem_btree_node();
        }
        mem_btree_node* insert(const vptr ptr, euint size, const char* name, destructor dest) {
			bool added = false;
            mem_btree_node* track_buffer[sizeof(ptr) * 2 + 1];
            mem_btree_node* node = root;
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
                mem_btree_node* next = node->get_child_node(section);
                if (!next) {
                    next = ENEW mem_btree_node();
                    next->section = section;
                    node->add_child_node(section, next);
					added = true;
                }
                node = next;
                shift -= 4;
                mask >>= 4;
            }
            track_buffer[0] = node;
            node->name = name;
            node->dest = dest;
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
            mem_btree_node* track_buffer[sizeof(ptr) * 2 + 1];
            mem_btree_node* node = root;
            euint num_bytes = sizeof(ptr);
            for (euint i = 0; i < num_bytes * 2; i++) {
                track_buffer[sizeof(ptr) * 2 - i] = node;
                mem_btree_node* next = node->get_child_node(ptr);
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
			if (node->dest) {
				node->dest(node->begin_addr);
			}
			alloced_size -= ((ref_ptr)node->end_addr - (ref_ptr)node->begin_addr);

            for (euint i = 1; i < num_bytes * 2 + 1; i++) {
                mem_btree_node* next = track_buffer[i];
                mem_btree_node* prev = track_buffer[i - 1];
                bool is_deleted = false;
                node = next->remove_child_node(prev->section, is_deleted);
                track_buffer[i - 1] = NULL;
                delete node;
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
		/**
		void remove(mem_btree_node* node) {
			euint num_bytes = sizeof(vptr);
			mem_btree_node* next = node->parent;
			mem_btree_node* prev = node;
			for (euint i = 1; i < num_bytes * 2 + 1; i++) {
				bool is_deleted = false;
				node = next->remove_child_node(prev->section, is_deleted);
				prev = next;
				next = next->parent;
				delete node;
				if (!is_deleted)
					break;
			}
			while (next) {
				node = next;
				node->remove_update();
                next = node->parent;
			}
			count--;
		}
		**/
        mem_btree_node* find(const vptr ptr) {
            mem_btree_node* node = root;
            euint num_bytes = sizeof(ptr);
            for (euint i = 0; i < num_bytes * 2; i++) {
                mem_btree_node* next = node->get_child_node(ptr);
                if (!next)
                    return NULL;
                node = next;
            }
            return node;
        }
		euint size() {
			return count;
		}
        void push_detach_node(mem_btree_node* node);
		euint get_alloced_size() {
			return alloced_size;
		}
    };
}

#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */