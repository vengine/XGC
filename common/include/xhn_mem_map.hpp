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

#include "xhn_btree.hpp"

#include "xhn_config.hpp"

namespace xhn
{
    typedef void (*destructor) (void*);
    class mem_btree_node : public btree_node<vptr, ref_ptr>
    {
    public:
        struct data
        {
            const char* name;
            destructor dest;
        };
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
    public:
        mem_btree_node()
        : root_ref_count(0)
		, prev( NULL )
		, next( NULL )
		, root_prev( NULL )
		, root_next( NULL )
        , name( NULL )
        , orphan_count( 100 )
        , dest( NULL )
		, is_garbage(false)
        {}

		void Attach(const vptr handle, mem_btree_node* mem);
		void Detach(const vptr handle, mem_btree_node* mem);
		void AttchToRoot();
		void DetachFromRoot();
		void MarkNotGarbage();
        bool _TrackBack(mem_set& trackBuffer);
        void _Erase();
        bool TrackBack();
    };
    
    class FMemNodeAllocator
    {
    public:
        FMemNodeAllocator() {}
        
        void deallocate(mem_btree_node* ptr, euint)
        {
            UnlockedMemAllocator_free(mem_btree_node::s_allocator, ptr);
        }
        
        mem_btree_node* allocate(euint count)
        {
            if (!mem_btree_node::s_allocator) {
                mem_btree_node::s_allocator = UnlockedMemAllocator_new();
            }
            return (mem_btree_node*)UnlockedMemAllocator_alloc(
                        mem_btree_node::s_allocator,
                        count * sizeof(mem_btree_node),
                        false);
        }
        
        void construct(mem_btree_node* ptr, vptr data)
        {
            new ( ptr ) mem_btree_node ();
            if (data) {
                mem_btree_node::data* node_data = (mem_btree_node::data*)data;
                ptr->name = node_data->name;
                ptr->dest = node_data->dest;
            }
        }
        
        void pre_destroy(mem_btree_node* ptr)
        {
            if (ptr->dest) {
				ptr->dest(ptr->begin_addr);
			}
        }
        void destroy(mem_btree_node* ptr)
        {
            ptr->~mem_btree_node();
        }
    };
    
    class mem_btree : public btree<mem_btree_node, vptr, ref_ptr, FMemNodeAllocator>
    {
    public:
        mem_btree_node* insert(const vptr ptr, euint size, const char* name, destructor dest) {
            mem_btree_node::data data = {name, dest};
            return (mem_btree_node*)btree<mem_btree_node,
            vptr,
            ref_ptr,
            FMemNodeAllocator>::insert(ptr, size, &data);
        }
        void push_detach_node(mem_btree_node* node);
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