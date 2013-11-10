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
/**
    class mem_btree_node :
    public btree_node<
    vptr,
    ref_ptr,
    UnlockedMapNodeAllocator< range<vptr>, vptr >
    >
    {
    public:
        struct data
        {
            const char* name;
            destructor dest;
        };
	public:
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
					UnlockedSetNodeAllocator<input_pair>> input_mem_set;
        typedef map<vptr, 
			        mem_btree_node*,
		            FLessProc<vptr>,
					UnlockedMapNodeAllocator<vptr, mem_btree_node*>> mem_map;
		typedef set<mem_btree_node*,
			        FLessProc<mem_btree_node*>, 
			        UnlockedSetNodeAllocator<mem_btree_node*>> mem_set;
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
            UnlockedMemAllocator_free(s_unlocked_allocator, ptr);
        }
        
        mem_btree_node* allocate(euint count)
        {
            if (!s_unlocked_allocator) {
                s_unlocked_allocator = UnlockedMemAllocator_new();
            }
            return (mem_btree_node*)UnlockedMemAllocator_alloc(
                        s_unlocked_allocator,
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
**/
}

namespace xhn
{
    class mem_node
    {
    public:
        typedef range_map<vptr, mem_node> range_mem_map;
        typedef range_mem_map::node_ptr mem_node_ptr;
        struct data
        {
            const char* name;
            destructor dest;
        };
    public:
        inline mem_node* _to_node(mem_node_ptr ptr) {
            return &ptr->second;
        }
        inline mem_node_ptr _to_ptr(mem_node* node) {
            return (mem_node_ptr)((ref_ptr)node - (ref_ptr)&((mem_node_ptr)0)->second);
        }
	public:
		struct input_pair
		{
			mem_node* node;
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
        UnlockedSetNodeAllocator<input_pair>> input_mem_set;
        typedef map<vptr,
        mem_node*,
        FLessProc<vptr>,
        UnlockedMapNodeAllocator<vptr, mem_node*>> mem_map;
		typedef set<mem_node*,
        FLessProc<mem_node*>,
        UnlockedSetNodeAllocator<mem_node*>> mem_set;
    public:
		input_mem_set input_set;
		mem_map output_map;
		esint32 root_ref_count;
		mem_node* prev;
		mem_node* next;
		mem_node* root_prev;
		mem_node* root_next;
        const char* name;
        esint32 orphan_count;
        destructor dest;
		bool is_garbage;
    public:
        mem_node()
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
        ~mem_node();
		void Attach(const vptr handle, mem_node* mem);
		void Detach(const vptr handle, mem_node* mem);
		void AttchToRoot();
		void DetachFromRoot();
		void MarkNotGarbage();
        bool _TrackBack(mem_set& trackBuffer);
        void _Erase();
        bool TrackBack();
    };
    
    class mem_map : public range_map<vptr, mem_node>
    {
    public:
        euint m_alloced_size;
    public:
        mem_map()
        : m_alloced_size(0)
        {}
        mem_node* insert(const vptr ptr, euint size, const char* name, destructor dest) {
            mem_node node;
            node.name = name;
            node.dest = dest;
            vptr end_addr = (vptr)((ref_ptr)ptr + (ref_ptr)size - 1);
            return range_map<vptr, mem_node>::insert(ptr, end_addr, node);
        }
        void push_detach_node(mem_node* node);
        inline euint get_alloced_size() const {
            return m_alloced_size;
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