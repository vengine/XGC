/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_VECTOR_HPP
#define XHN_VECTOR_HPP
#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "emem.hpp"
#include <new>

#include "xhn_iterator.hpp"
#include "xhn_utility.hpp"
namespace xhn
{
template < typename T,
           typename GET_ELEM_REAL_SIZE = FGetElementRealSizeProc<T>,
           typename CTOR = FCtorProc<T>,
           typename DEST = FDestProc<T> >
class vector : public RefObject
{
public:
    struct FReadProc {
        void operator() ( T *from, T &to ) {
            to = *from;
        }
    };
    struct FWriteProc {
        void operator() ( T *to, T &from ) {
            *to = from;
        }
    };
    struct FNextProc {
        void operator() ( T *from, T *&to, euint ele_real_size ) {
            to = ( T * ) ( ( char * ) from + ele_real_size );
        }
    };
    struct FPrevProc {
        void operator() ( T *from, T *&to, euint ele_real_size ) {
            to = ( T * ) ( ( char * ) from - ele_real_size );
        }
    };
    template <typename Owner>
    struct FRedirectProc {
        void operator() ( Owner *owner, T *from, T *&to, euint ele_real_size, esint offset ) {
            char *begin = owner->m_begin_addr;
            char *end = owner->m_barrier;
            char *ptr = ( char * ) from;
            ptr += offset * ( esint ) ele_real_size;

            if ( ptr < begin ) {
                ptr = begin;
            }

            if ( ptr > end ) {
                ptr = end;
            }

            to = ptr;
        }
    };

    class iterator : public random_readwrite_iterator<T, FReadProc, FWriteProc, FNextProc, FPrevProc, FRedirectProc<vector>, vector>
    {
    public:
        typedef random_readwrite_iterator<T, FReadProc, FWriteProc, FNextProc, FPrevProc, FRedirectProc<vector>, vector> base_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        iterator ( vector<T, GET_ELEM_REAL_SIZE> *owner, char *a, euint ele_real_size )
            : base_type
            ( ( T * ) a, ele_real_size, FReadProc(), FWriteProc(), FNextProc(), FPrevProc(), FRedirectProc<vector>(), owner )
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
        inline iterator operator + ( esint offs ) {
            iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline iterator operator - ( esint offs ) {
            iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
    };
    class const_iterator : public const_random_readwrite_iterator<T, FReadProc, FWriteProc, FNextProc, FPrevProc, FRedirectProc<vector>, vector>
    {
    public:
        typedef const_random_readwrite_iterator<T, FReadProc, FWriteProc, FNextProc, FPrevProc, FRedirectProc<vector>, vector> base_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;
        const_iterator ( vector<T, GET_ELEM_REAL_SIZE> *owner, char *a, euint ele_real_size )
            : base_type
            ( ( T * ) a, ele_real_size, FReadProc(), FWriteProc(), FNextProc(), FPrevProc(), FRedirectProc<vector>(), owner )
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
        inline const_iterator operator + ( esint offs ) {
            const_iterator tmp = *this;
            tmp.redirect ( offs );
            return tmp;
        }
        inline const_iterator operator - ( esint offs ) {
            const_iterator tmp = *this;
            tmp.redirect ( -offs );
            return tmp;
        }
    };
    euint m_totel_ele_count;
    euint m_ele_real_size;
    char *m_begin_addr;
    char *m_barrier;
    GET_ELEM_REAL_SIZE m_get_elem_real_size;
    CTOR m_ctor;
    DEST m_dest;
    iterator begin() {
        return iterator ( this, m_begin_addr, m_ele_real_size );
    }
    iterator end() {
        return iterator ( this, m_barrier, m_ele_real_size );
    }
    const_iterator begin() const {
        return const_iterator ( (vector*)this, m_begin_addr, m_ele_real_size );
    }
    const_iterator end() const {
        return const_iterator ( (vector*)this, m_barrier, m_ele_real_size );
    }
    inline void push_back ( const T& v ) {
		T& n_v = (T&)v;
        euint curt_count = _get_size();

        if ( curt_count + 1 > m_totel_ele_count ) {
            reserve(m_totel_ele_count * 8);
        }

        m_ctor( (T*)m_barrier, n_v );
        m_barrier += m_ele_real_size;
    }
    inline void pop_back() {
        if ( m_barrier != m_begin_addr ) {
            m_dest(m_barrier);
            m_barrier -= m_ele_real_size;
        }
    }
    inline T &back() {
        euint i = size() - 1;
        euint offs = m_ele_real_size * i;
        char *ptr = m_begin_addr + offs;
        return * ( ( T * ) ptr );
    }
    inline T &front() {
        return * ( ( T * ) m_begin_addr );
    }
    inline const T &back() const {
        euint i = size() - 1;
        euint offs = m_ele_real_size * i;
        char *ptr = m_begin_addr + offs;
        return * ( ( T * ) ptr );
    }
    inline const T &front() const {
        return * ( ( T * ) m_begin_addr );
    }
    inline T &operator [] ( euint i ) {
        euint offs = m_ele_real_size * i;
        char *ptr = m_begin_addr + offs;
        return * ( ( T * ) ptr );
    }
    inline const T &operator [] ( euint i ) const {
        euint offs = m_ele_real_size * i;
        char *ptr = m_begin_addr + offs;
        return * ( ( T * ) ptr );
    }
    inline bool empty() const {
        return _get_size() == 0;
    }
    inline euint size() const {
        return _get_size();
    }
    inline euint _get_size() const {
        return ( m_barrier - m_begin_addr ) / m_ele_real_size;
    }
    inline void clear() {
        m_barrier = m_begin_addr;
    }
    inline void reserve(euint n) {
        if ( n > m_totel_ele_count ) {
            euint curt_count = _get_size();
            char *tmp = ( char * ) Malloc ( m_ele_real_size * n );
            char *dst_ptr = tmp;
            char *src_ptr = m_begin_addr;

            for ( euint i = 0; i < curt_count; i++ ) {
                m_ctor((T*)dst_ptr, * ( ( T * ) src_ptr ));
                dst_ptr += m_ele_real_size;
                src_ptr += m_ele_real_size;
            }

            src_ptr = m_begin_addr;

            for ( euint i = 0; i < curt_count; i++ ) {
                m_dest((T*)src_ptr);
                src_ptr += m_ele_real_size;
            }

            Mfree ( m_begin_addr );
            m_begin_addr = tmp;
            m_barrier = m_begin_addr + ( m_ele_real_size * curt_count );
            m_totel_ele_count = n;
        }
    }
    inline void resize(euint n) {
        reserve(n);
        euint curt_count = _get_size();
        euint d = n - curt_count;
        for (euint i = 0; i < d; i++) {
            m_ctor((T*)m_barrier);
            m_barrier += m_ele_real_size;
        }
    }
	inline void erase(iterator i) {
		if ((ref_ptr)i.m_ptr >= (ref_ptr)m_begin_addr && (ref_ptr)i.m_ptr < (ref_ptr)m_barrier) {
            m_dest(i.m_ptr);
			iterator prev_iter = i;
			i++;
			for (; i != end(); i++)
			{
				*prev_iter = *i;
			}
			m_dest((T*)m_barrier);
			m_barrier -= m_ele_real_size;
		}
	}
	inline iterator insert(iterator i, const T& v) {
		if ((ref_ptr)i.m_ptr >= (ref_ptr)m_begin_addr && (ref_ptr)i.m_ptr < (ref_ptr)m_barrier) {
			T& n_v = (T&)v;
			ref_ptr offset = (ref_ptr)i.m_ptr - (ref_ptr)m_begin_addr;
			reserve(_get_size() + 1);
			i = iterator(this, m_begin_addr + offset, m_ele_real_size);
			iterator src(this, m_barrier, m_ele_real_size);
			m_barrier += m_ele_real_size;
			m_ctor((T*)m_barrier);
			iterator dst(this, m_barrier, m_ele_real_size);
			for (; src != i; src--, dst--) {
                *dst = *src;
			}
			dst = i;
			dst++;
            *dst = *i;
			*i = n_v;
			return i;
		}
		return iterator(this, NULL, 0);
	}
	inline T* get() const {
		return (T*)m_begin_addr;
	}
	inline void _init(const vector& v) {
		euint size = m_get_elem_real_size();
		m_ele_real_size = size;
		m_totel_ele_count = v.m_totel_ele_count;
		m_begin_addr = ( char * ) Malloc ( size * m_totel_ele_count );
		m_barrier = m_begin_addr + (v.m_barrier - v.m_begin_addr);
		euint count = (m_barrier - m_begin_addr) / size;
		for (euint i = 0; i < count; i++) {
			euint offs = size * i;
			char *dst = m_begin_addr + offs;
			char *src = v.m_begin_addr + offs;
			m_ctor((T*)dst, *((T*)src));
		}
	}
	inline void _dest() {
		euint curt_count = _get_size();
		char *src_ptr = m_begin_addr;
		for ( euint i = 0; i < curt_count; i++ ) {
			m_dest((T*)src_ptr);
			src_ptr += m_ele_real_size;
		}
		Mfree ( m_begin_addr );
	}
	vector& operator = (const vector& v) {
        _dest();
		_init(v);
		return *this;
	}
    vector() {
        euint size = m_get_elem_real_size();
        m_begin_addr = ( char * ) Malloc ( size * 32 );
        m_barrier = m_begin_addr;
        m_ele_real_size = size;
        m_totel_ele_count = 32;
    }
	vector(const vector& v) {
		_init(v);
	}
    ~vector() {
        _dest();
    }
};
};
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
