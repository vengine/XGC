/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_ITERATOR_HPP
#define XHN_ITERATOR_HPP
#include "common.h"
#include "etypes.h"
#include "emem.h"
#include <new>
namespace xhn
{
template <typename T, typename Readproc>
class readonly_iterator
{
protected:
    const T *m_ptr;
    Readproc m_readProc;
public:
    readonly_iterator ( const T *ptr, Readproc readProc )
        : m_ptr ( ptr )
        , m_readProc ( readProc )
    {}
    inline void read ( T &to ) const {
        m_readProc ( m_ptr, to );
    }
};
template <typename T, typename Writeproc>
class writeonly_iterator
{
protected:
    T *m_ptr;
    Writeproc m_writeProc;
public:
    writeonly_iterator ( T *ptr, Writeproc writeProc )
        : m_ptr ( ptr )
        , m_writeProc ( writeProc )
    {}
    inline void write ( const T &from ) {
        m_writeProc ( m_ptr, from );
    }
};
template <typename T, typename Readproc, typename Writeproc>
class readwrite_iterator
{
public:
    T *m_ptr;
    Readproc m_readProc;
    Writeproc m_writeProc;
public:
    readwrite_iterator ( T *ptr, Readproc readProc, Writeproc writeProc )
        : m_ptr ( ptr )
        , m_readProc ( readProc )
        , m_writeProc ( writeProc )
    {}
    inline void read ( T &to ) const {
        m_readProc ( m_ptr, to );
    }
    inline void write ( const T &from ) {
        m_writeProc ( m_ptr, from );
    }
    inline bool operator == ( const readwrite_iterator &rts ) const {
        return m_ptr == rts.m_ptr;
    }
    inline bool operator != ( const readwrite_iterator &rts ) const {
        return m_ptr != rts.m_ptr;
    }
    inline T &operator*() {
        return *m_ptr;
    }
    inline T *operator->() {
        return m_ptr;
    }
	inline bool is_valid() {
		return m_ptr != NULL;
	}
	inline T & get() {
		return *m_ptr;
	}
};

template <typename T, typename Readproc, typename Writeproc>
class const_readwrite_iterator
{
public:
    T *m_ptr;
    Readproc m_readProc;
    Writeproc m_writeProc;
public:
    const_readwrite_iterator ( T *ptr, Readproc readProc, Writeproc writeProc )
        : m_ptr ( ptr )
        , m_readProc ( readProc )
        , m_writeProc ( writeProc )
    {}
    inline void read ( T &to ) const {
        m_readProc ( m_ptr, to );
    }
    inline bool operator == ( const const_readwrite_iterator &rts ) const {
        return m_ptr == rts.m_ptr;
    }
    inline bool operator != ( const const_readwrite_iterator &rts ) const {
        return m_ptr != rts.m_ptr;
    }
    inline const T &operator*() const {
        return *m_ptr;
    }
    inline const T *operator->() const {
        return m_ptr;
    }
	inline bool is_valid() {
		return m_ptr != NULL;
	}
	inline const T & get() {
		return *m_ptr;
	}
};

template <typename T, typename Readproc, typename Writeproc, typename Nextproc>
class forward_readwrite_iterator : public readwrite_iterator<T, Readproc, Writeproc>
{
public:
    typedef readwrite_iterator<T, Readproc, Writeproc> base_type;
    euint m_ele_real_size;
    Nextproc m_nextProc;
public:
    forward_readwrite_iterator ( T *ptr, euint ele_real_size, Readproc readProc, Writeproc writeProc, Nextproc nextProc )
        : base_type ( ptr, readProc, writeProc )
        , m_ele_real_size ( ele_real_size )
        , m_nextProc ( nextProc )
    {}
    inline void next() {
        m_nextProc (
            base_type::m_ptr,
            base_type::m_ptr,
            m_ele_real_size );
    }
};

template <typename T, typename Readproc, typename Writeproc, typename Nextproc>
class const_forward_readwrite_iterator : public const_readwrite_iterator<T, Readproc, Writeproc>
{
public:
    typedef const_readwrite_iterator<T, Readproc, Writeproc> base_type;
    euint m_ele_real_size;
    Nextproc m_nextProc;
public:
    const_forward_readwrite_iterator ( T *ptr, euint ele_real_size, Readproc readProc, Writeproc writeProc, Nextproc nextProc )
        : base_type ( ptr, readProc, writeProc )
        , m_ele_real_size ( ele_real_size )
        , m_nextProc ( nextProc )
    {}
    inline void next() {
        m_nextProc (
            base_type::m_ptr,
            base_type::m_ptr,
            m_ele_real_size );
    }
};

template <typename T, typename Readproc, typename Writeproc, typename Nextproc, typename Prevproc>
class bidirectional_readwrite_iterator : public forward_readwrite_iterator<T, Readproc, Writeproc, Nextproc>
{
public:
    typedef forward_readwrite_iterator<T, Readproc, Writeproc, Nextproc> base_type;
    Prevproc m_prevProc;
public:
    bidirectional_readwrite_iterator ( T *ptr, euint ele_real_size, Readproc readProc, Writeproc writeProc, Nextproc nextProc, Prevproc prevProc )
        : base_type ( ptr, ele_real_size, readProc, writeProc, nextProc )
        , m_prevProc ( prevProc )
    {}
    inline void prev() {
        m_prevProc (
            base_type::m_ptr,
            base_type::m_ptr,
            base_type::m_ele_real_size );
    }
};

template <typename T, typename Readproc, typename Writeproc, typename Nextproc, typename Prevproc>
class const_bidirectional_readwrite_iterator : public const_forward_readwrite_iterator<T, Readproc, Writeproc, Nextproc>
{
public:
    typedef const_forward_readwrite_iterator<T, Readproc, Writeproc, Nextproc> base_type;
    Prevproc m_prevProc;
public:
    const_bidirectional_readwrite_iterator ( T *ptr, euint ele_real_size, Readproc readProc, Writeproc writeProc, Nextproc nextProc, Prevproc prevProc )
        : base_type ( ptr, ele_real_size, readProc, writeProc, nextProc )
        , m_prevProc ( prevProc )
    {}
    inline void prev() {
        m_prevProc (
            base_type::m_ptr,
            base_type::m_ptr,
            base_type::m_ele_real_size );
    }
};

template <typename T, typename Readproc, typename Writeproc, typename Nextproc, typename Prevproc, typename Redirectproc, typename Owner>
class random_readwrite_iterator : public bidirectional_readwrite_iterator<T, Readproc, Writeproc, Nextproc, Prevproc>
{
public:
    typedef bidirectional_readwrite_iterator<T, Readproc, Writeproc, Nextproc, Prevproc> base_type;
    Redirectproc m_redirectProc;
    Owner *m_owner;
public:
    random_readwrite_iterator ( T *ptr, euint ele_real_size, Readproc readProc, Writeproc writeProc, Nextproc nextProc, Prevproc prevProc, Redirectproc redirectProc, Owner *owner )
        : base_type ( ptr, ele_real_size, readProc, writeProc, nextProc, prevProc )
        , m_redirectProc ( redirectProc )
        , m_owner ( owner )
    {}
    inline void redirect ( esint offset ) {
        m_redirectProc (
            base_type::m_owner,
            base_type::m_ptr,
            base_type::m_ptr,
            base_type::m_ele_real_size, offset );
    }
};

template <typename T, typename Readproc, typename Writeproc, typename Nextproc, typename Prevproc, typename Redirectproc, typename Owner>
class const_random_readwrite_iterator : public const_bidirectional_readwrite_iterator<T, Readproc, Writeproc, Nextproc, Prevproc>
{
public:
    typedef const_bidirectional_readwrite_iterator<T, Readproc, Writeproc, Nextproc, Prevproc> base_type;
    Redirectproc m_redirectProc;
    Owner *m_owner;
public:
    const_random_readwrite_iterator ( T *ptr, euint ele_real_size, Readproc readProc, Writeproc writeProc, Nextproc nextProc, Prevproc prevProc, Redirectproc redirectProc, Owner *owner )
        : base_type ( ptr, ele_real_size, readProc, writeProc, nextProc, prevProc )
        , m_redirectProc ( redirectProc )
        , m_owner ( owner )
    {}
    inline void redirect ( esint offset ) {
        m_redirectProc ( m_owner,
                         base_type::m_ptr,
                         base_type::m_ptr,
                         base_type::m_ele_real_size, offset );
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
