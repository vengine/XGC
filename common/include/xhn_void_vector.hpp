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
				reserve(m_totel_ele_count * 8);
			}

			memcpy(m_barrier, _ele, m_ele_size);
		    m_barrier += m_ele_size;
		}
		inline void resize(euint n) {
			reserve(n);
			euint curt_count = _get_size();
			euint d = n - curt_count;
			m_barrier += m_ele_size * d;
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
	};
}
#endif