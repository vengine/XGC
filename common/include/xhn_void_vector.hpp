/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_VOID_VECTOR_HPP
#define XHN_VOID_VECTOR_HPP
#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "emem.hpp"
#include <new>

#include "xhn_iterator.hpp"
#include "xhn_utility.hpp"
namespace xhn
{
    class void_vector : public RefObject
	{
	public:
		struct FCharProc {
			void operator() ( vptr to, vptr from ) {
			}
		};
	public:
		euint m_ele_size;
		euint m_totel_ele_count;
		char* m_begin_addr;
		char* m_barrier;
		void_vector()
		{
			m_begin_addr = NULL;
			m_barrier = NULL;
			m_ele_size = 0;
			m_totel_ele_count = 0;
		}
		~void_vector()
		{
            Mfree(m_begin_addr);
		}
        inline void_vector& operator =(const void_vector& v) {
            m_ele_size = v.m_ele_size;
            resize(v._get_size());
            memcpy(m_begin_addr, v.m_begin_addr, m_ele_size * size());
            return *this;
        }
		inline euint _get_size() const {
			if (m_ele_size)
			    return ( m_barrier - m_begin_addr ) / m_ele_size;
			else
				return 0;
		}
		inline euint size() const {
			return _get_size();
		}
        inline euint element_size() const {
            return m_ele_size;
        }
		inline void reserve(euint n) {
			if ( n > m_totel_ele_count ) {
				euint curt_count = _get_size();
				char *tmp = ( char * ) SMalloc ( m_ele_size * n );
				if (m_begin_addr) {
					char *dst_ptr = tmp;
					char *src_ptr = m_begin_addr;

					for ( euint i = 0; i < curt_count; i++ ) {
						memcpy(dst_ptr, src_ptr, m_ele_size);
						dst_ptr += m_ele_size;
						src_ptr += m_ele_size;
					}

					Mfree ( m_begin_addr );
				}
				m_begin_addr = tmp;
				m_barrier = m_begin_addr + ( m_ele_size * curt_count );
				m_totel_ele_count = n;
			}
		}
		inline void push_back(vptr _ele) {
			euint curt_count = _get_size();

			if ( curt_count + 1 > m_totel_ele_count ) {
				reserve((curt_count + 1) * 8);
			}

			memcpy(m_barrier, _ele, m_ele_size);
		    m_barrier += m_ele_size;
		}
		inline void resize(euint n) {
            if (n > m_totel_ele_count) {
                reserve(n);
                euint curt_count = _get_size();
                euint d = n - curt_count;
                m_barrier += m_ele_size * d;
            }
			else {
                m_barrier = m_begin_addr + ( m_ele_size * n );
            }
		}
		inline void flush(vptr pxl) {
			char* ptr = m_begin_addr;
			while (ptr != m_barrier) {
				memcpy(ptr, pxl, m_ele_size);
				ptr += m_ele_size;
			}
		}
		inline vptr operator [] ( euint i ) {
			euint offs = m_ele_size * i;
			char *ptr = m_begin_addr + offs;
			return (vptr)ptr;
		}
		inline const vptr operator [] ( euint i ) const {
			euint offs = m_ele_size * i;
			char *ptr = m_begin_addr + offs;
			return (vptr)ptr;
		}
		template <typename CONVERT_PROC>
		void convert(euint _ele_size) {
            CONVERT_PROC conv_proc;
            m_ele_size = _ele_size;
			euint curt_count = _get_size();
			char* tmp = (char*)SMalloc(_ele_size * m_totel_ele_count);
			if (m_begin_addr) {
				for (euint i = 0; i < curt_count; i++) {
					euint src_offs = m_ele_size * i;
					char* src_ptr = m_begin_addr + src_offs;
					euint dst_offs = _ele_size * i;
					char* dst_ptr = tmp + dst_offs;
					conv_proc(dst_ptr, src_ptr);
				}
				Mfree(m_begin_addr);
			}
			m_begin_addr = tmp;
			m_barrier = m_begin_addr + curt_count * _ele_size;
		}
		void clear() {
			if (m_begin_addr) {
				Mfree(m_begin_addr);
			}
			m_begin_addr = NULL;
			m_barrier = NULL;
			m_ele_size = 0;
			m_totel_ele_count = 0;
		}
		vptr get() {
			return m_begin_addr;
		}
		const vptr get() const {
			return m_begin_addr;
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