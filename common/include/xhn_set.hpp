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
struct FSetNodeCreateProc {
	rbtree_node<K> *operator() () {
		rbtree_node<K> *ret = ( rbtree_node<K> * ) Malloc ( sizeof ( rbtree_node<K> ) );
		new (ret) rbtree_node<K>();
		return ret;
	}
};
template <typename K>
struct FSetNodeDeleteProc {
	void operator() (rbtree_node<K> * n) {
		n->~rbtree_node<K>();
	}
};
template <typename K>
struct FSetNodeFreeProc {
	void operator() (vptr n) {
		Mfree ( n );
	}
};

template <typename K>
class FSetNodeAllocProc
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
		typedef FSetNodeAllocProc<AK> other;
	};

	pointer address(reference v) const
	{	
		return &v;
	}

	const_pointer address(const_reference v) const
	{	
		return &v;
	}

	FSetNodeAllocProc()
	{
	}

	FSetNodeAllocProc(const FSetNodeAllocProc& rth) 
	{
	}

	template<typename AK>
	FSetNodeAllocProc(const FSetNodeAllocProc<AK>&)
	{
	}

	template<typename AK>
	FSetNodeAllocProc<AK>& operator=(const FSetNodeAllocProc<AK>&)
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
          typename NodeAllocproc = FSetNodeAllocProc<K> >
class set : public RefObject
{
private:
    rbtree<K, K, LessProc, NodeAllocproc> m_rbtree;
public:
    typedef rbtree<K, K, LessProc, NodeAllocproc> rbtree_type;
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
