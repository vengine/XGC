//
//  xhn_mem_map2.h
//  Xgc
//
//  Created by 徐 海宁 on 13-11-10.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#ifndef __Xgc__xhn_mem_map2__
#define __Xgc__xhn_mem_map2__

#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "emem.hpp"
#include <new>

#include "xhn_iterator.hpp"
#include "xhn_utility.hpp"
#include "xhn_rbtree.hpp"
#include "xhn_pair.hpp"

#include "xhn_set.hpp"
#include "xhn_lock.hpp"
#include "xhn_atomic_operation.hpp"
#include "rwbuffer.h"
namespace xhn
{
    class mem_info : public pair<vptr, euint>
    {
    public:
        struct FMemInfoNodeCreateProc {
            rbtree_node<mem_info*>* operator() ();
        };
        struct FMemInfoNodeDeleteProc {
            void operator() (rbtree_node<mem_info*>* n);
        };
        struct FMemInfoNodeFreeProc {
            void operator() (vptr n);
        };
        class FMemInfoNodeAllocProc
        {
        public:
            typedef euint size_type;
            typedef euint difference_type;
            typedef rbtree_node<mem_info*>* pointer;
            typedef const rbtree_node<mem_info*>* const_pointer;
            typedef rbtree_node<mem_info*> value_type;
            typedef rbtree_node<mem_info*>& reference;
            typedef const rbtree_node<mem_info*>& const_reference;
            
            pointer address(reference v) const
            {
                return &v;
            }
            
            const_pointer address(const_reference v) const
            {
                return &v;
            }
            
            FMemInfoNodeAllocProc()
            {
            }
            
            FMemInfoNodeAllocProc(const FMemInfoNodeAllocProc& rth)
            {
            }
            
            FMemInfoNodeAllocProc& operator=(const FMemInfoNodeAllocProc&)
            {
                return (*this);
            }
            
            void deallocate(pointer ptr, size_type);
            pointer allocate(size_type count);
            pointer allocate(size_type count, const void*);
            void construct(pointer ptr);
            void destroy(pointer ptr);
            
            size_type max_size() const {
                return static_cast<size_type>(-1) / sizeof(value_type);
            }
        };
    public:
        mem_info ( const vptr f, const euint s )
        : pair<vptr, euint>(f, s)
        {}
        mem_info ( const mem_info &p )
        : pair<vptr, euint>(p)
        {}
        mem_info ()
        {}
        mem_info &operator = ( const mem_info &p ) {
            pair<vptr, euint>::operator =(p);
            input_set = p.input_set;
            output_set = p.output_set;
            is_root = p.is_root;
            return *this;
        }
    };
    
    class mem_map : public RefObject
    {
        friend class mem_info::FMemInfoNodeAllocProc;
    private:
        static MemAllocator s_allocator;
    private:
        typedef rbtree_node<vptr> mem_node;
        struct FMemInfoNodeSpecProc
        {
            vptr addr;
            FMemInfoNodeSpecProc()
            : addr(NULL)
            {}
            bool operator () (rbtree_node<vptr>* node) const {
                if (!addr)
                    return false;
                mem_info* minfo = (mem_info*)node;
                if ((ref_ptr)addr >= (ref_ptr)minfo->first &&
                    (ref_ptr)addr <= (ref_ptr)minfo->first + (ref_ptr)minfo->second)
                    return true;
                else
                    return false;
            }
        };
        class FNodeAllocProc
        {
        public:
            typedef euint size_type;
            typedef euint difference_type;
            typedef mem_node* pointer;
            typedef const mem_node* const_pointer;
            typedef mem_node value_type;
            typedef mem_node& reference;
            typedef const mem_node& const_reference;
            
            pointer address(reference v) const
            {
                return &v;
            }
            
            const_pointer address(const_reference v) const
            {
                return &v;
            }
            
            FNodeAllocProc()
            {
            }
            
            FNodeAllocProc(const FNodeAllocProc& rth)
            {
            }
            
            FNodeAllocProc& operator=(const FNodeAllocProc&)
            {
                return (*this);
            }
            
            void deallocate(pointer ptr, size_type) {
                MemAllocator_free( s_allocator, ptr );
            }
            pointer allocate(size_type count) {
                mem_info *ret = ( mem_info * ) MemAllocator_alloc ( s_allocator, sizeof ( mem_info ), false );
                return (mem_node*)ret;
            }
            pointer allocate(size_type count, const void*) {
                mem_info *ret = ( mem_info * ) MemAllocator_alloc ( s_allocator, sizeof ( mem_info ), false );
                return (mem_node*)ret;
            }
            void construct(pointer ptr, const vptr& v) {
                new (ptr) mem_info();
            }
            void construct(pointer ptr) {
                new (ptr) mem_info();
            }
            void destroy(pointer ptr) {
                ((mem_info*)ptr)->~mem_info();
            }
            
            size_type max_size() const {
                return static_cast<size_type>(-1) / sizeof(value_type);
            }
        };
    private:
        typedef rbtree<vptr, euint, FLessProc<vptr>, FNodeAllocProc, FMemInfoNodeSpecProc> rbtree_type;
    private:
        rbtree_type m_rbtree;
        SpinLock m_lock;
    public:
        template<typename T>
        struct FReadProc {
            void operator() ( T *from, T &to ) {
                to = *from;
            }
        };
        template<typename T>
        struct FWriteProc {
            void operator() ( T *to, T &from ) {
                *to = from;
            }
        };
        template<typename T>
        struct FNextProc {
            void operator() ( T *from, T *&to, euint ele_real_size ) {
                to = ( T * ) from->iter_next;
            }
        };
        template<typename T>
        struct FPrevProc {
            void operator() ( T *from, T *&to, euint ele_real_size ) {
                to = ( T * ) from->iter_prev;
            }
        };
    public:
        class iterator : public bidirectional_readwrite_iterator<mem_info, FReadProc<mem_info>, FWriteProc<mem_info>, FNextProc<mem_info>, FPrevProc<mem_info> >
        {
        private:
            SpinLock* lock;
        public:
            typedef bidirectional_readwrite_iterator<mem_info, FReadProc<mem_info>, FWriteProc<mem_info>, FNextProc<mem_info>, FPrevProc<mem_info> > base_type;
            iterator ( mem_info *a, euint ele_real_size,  SpinLock* lk)
            : base_type (
                         a, ele_real_size, FReadProc<mem_info>(), FWriteProc<mem_info>(), FNextProc<mem_info>(), FPrevProc<mem_info>() )
            , lock(lk)
            {}
            inline iterator &operator++() {
                SpinLock::Instance inst = lock->Lock();
                base_type::next();
                return *this;
            }
            inline iterator operator++ ( int ) {
                iterator tmp = *this;
                ++*this;
                return tmp;
            }
            inline iterator &operator--() {
                SpinLock::Instance inst = lock->Lock();
                base_type::prev();
                return *this;
            }
            inline iterator operator-- ( int ) {
                iterator tmp = *this;
                --*this;
                return tmp;
            }
        };
        
        mem_map()
        {
            if (!s_allocator.self) {
                s_allocator = MemAllocator_new();
            }
        }
        
        mem_info * insert ( const vptr p, euint s ) {
            SpinLock::Instance inst = m_lock.Lock();
            vptr key = (vptr)p;
            m_rbtree.specProc.addr = NULL;
            mem_info *info = ( mem_info * ) rbtree_type::_find ( &m_rbtree, key, true );
            info->second = s;
            return info;
        }
        iterator find ( const vptr p ) {
            SpinLock::Instance inst = m_lock.Lock();
            m_rbtree.specProc.addr = p;
            mem_info *info = (mem_info *)rbtree_type::_search_auxiliary( & m_rbtree, p, m_rbtree.root, NULL );
            if ( info )
                return iterator ( info, 0, &m_lock );
            else {
                return iterator ( NULL, 0, &m_lock );
            }
        }
        euint erase ( const vptr p ) {
            SpinLock::Instance inst = m_lock.Lock();
            m_rbtree.specProc.addr = p;
            return rbtree_type::_remove ( &m_rbtree, p );
        }
        iterator begin() {
            return iterator ( ( mem_info * ) m_rbtree.head, 0, &m_lock );
        }
        iterator end() {
            return iterator ( NULL, 0, &m_lock );
        }
        euint size() const {
            return m_rbtree.count;
        }
        void clear() {
            SpinLock::Instance inst = m_lock.Lock();
            m_rbtree.clear();
        }
        bool empty() const {
            return m_rbtree.count == 0;
        }
    };
}

#endif /* defined(__Xgc__xhn_mem_map2__) */
