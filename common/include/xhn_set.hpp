/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_SET_HPP
#define XHN_SET_HPP
#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "emem.hpp"
#include <new>

#include "xhn_iterator.hpp"
#include "xhn_utility.hpp"
#include "xhn_rbtree.hpp"

namespace xhn
{

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
		Mfree(ptr);
	}

	pointer allocate(size_type count)
	{
		return (pointer)Malloc(count * sizeof(value_type));
	}

	pointer allocate(size_type count, const void*)
	{
		return (pointer)Malloc(count * sizeof(value_type));
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

template< typename K, 
          typename LessProc = FLessProc<K>, 
          typename NodeAllocator = FSetNodeAllocator<K> >
class set : public RefObject
{
private:
    rbtree<K, K, LessProc, NodeAllocator> m_rbtree;
public:
    typedef rbtree<K, K, LessProc, NodeAllocator> rbtree_type;
    typedef rbtree_node<K> rbnode_type;
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
            rbnode_type *node = to_rbtree_node ( from );
            rbnode_type *next_node = node->iter_next;
            to = &next_node->first;
        }
    };
    template<typename T>
    struct FPrevProc {
        void operator() ( T *from, T *&to, euint ele_real_size ) {
            rbnode_type *node = to_rbtree_node ( from );
            rbnode_type *prev_node = node->iter_prev;
            to = &prev_node->first;
        }
    };
public:
    class iterator : public bidirectional_readwrite_iterator<K, FReadProc<rbnode_type>, FWriteProc<rbnode_type>, FNextProc<K>, FPrevProc<K> >
    {
    public:
        typedef bidirectional_readwrite_iterator<K, FReadProc<rbnode_type>, FWriteProc<rbnode_type>, FNextProc<K>, FPrevProc<K> > base_type;
        iterator ( K *a, euint ele_real_size )
            : base_type (
                a, ele_real_size, FReadProc<rbnode_type>(), FWriteProc<rbnode_type>(), FNextProc<K>(), FPrevProc<K>() )
        {}
        inline iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline iterator operator++ ( int ) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline iterator operator-- ( int ) {
            iterator tmp = *this;
            --*this;
            return tmp;
        }
    };

    class const_iterator : public const_bidirectional_readwrite_iterator<K, FReadProc<rbnode_type>, FWriteProc<rbnode_type>, FNextProc<K>, FPrevProc<K> >
    {
    public:
        typedef const_bidirectional_readwrite_iterator<K, FReadProc<rbnode_type>, FWriteProc<rbnode_type>, FNextProc<K>, FPrevProc<K> > base_type;
        const_iterator ( K *a, euint ele_real_size )
            : base_type (
                a, ele_real_size, FReadProc<rbnode_type>(), FWriteProc<rbnode_type>(), FNextProc<K>(), FPrevProc<K>() )
        {}
        inline const_iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline const_iterator operator++ ( int ) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline const_iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline const_iterator operator-- ( int ) {
            const_iterator tmp = *this;
            --*this;
            return tmp;
        }
    };

	set() {}
	set(const set& s) {
		const_iterator iter = s.begin();
		const_iterator end = s.end();
		for (; iter != end; iter++) {
            insert(*iter);
		}
	}
	set& operator = (const set& s) {
		clear();
		const_iterator iter = s.begin();
		const_iterator end = s.end();
		for (; iter != end; iter++) {
			insert(*iter);
		}
		return *this;
	}

    void insert ( K key ) {
        rbtree_type::_find ( &m_rbtree, key, true );
    }
    iterator find ( K &key ) {
        rbnode_type *node = ( rbnode_type * ) rbtree_type::_find ( &m_rbtree, key, false );
        K *k = &node->first;
        return iterator ( k, 0 );
    }
    const_iterator find ( const K &key ) const {
        rbnode_type *node = ( rbnode_type * ) rbtree_type::_find ( &m_rbtree, key, false );
        K *k = &node->first;
        return const_iterator ( k, 0 );
    }
    euint erase ( const K &key ) {
        return rbtree_type::_remove ( &m_rbtree, key );
    }
    euint erase ( iterator &iter ) {
        return rbtree_type::_remove ( &m_rbtree, *iter.m_ptr );
    }
    iterator erase ( const_iterator &iter ) {
        iterator ret ( iter->iter_next, 0 );
        rbtree_type::_remove ( &m_rbtree, *iter.m_ptr );
        return ret;
    }
    iterator begin() {
        K *k = &m_rbtree.head->first;
        return iterator ( k, 0 );
    }
    iterator end() {
        return iterator ( NULL, 0 );
    }
    const_iterator begin() const {
        K *k = &m_rbtree.head->first;
        return const_iterator ( k, 0 );
    }
    const_iterator end() const {
        return const_iterator ( NULL, 0 );
    }
    euint size() const {
        return m_rbtree.count;
    }
    void clear() {
        m_rbtree.clear();
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
