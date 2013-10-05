/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_HASH_SET_H
#define XHN_HASH_SET_H
#include "common.h"
#include "etypes.h"
#include "xhn_utility.hpp"
#include "xhn_string.hpp"
#include "xhn_list.hpp"
#include "xhn_lock.hpp"
#define HASH_BUCKET_COUNT (1<<15)
#define HASH_MASK         (HASH_BUCKET_COUNT - 1)
namespace xhn
{
template<typename T>
class hash_set : public RefObject
{
public:
    class bucket
    {
    public:
        list<T> m_value_list;
		SpinLock m_lock;
    public:
        typename list<T>::iterator insert ( const T &value ) {
			SpinLock::Instance inst = m_lock.Lock();
            if ( !m_value_list.size() ) {
                return m_value_list.push_back ( value );
            } else {
                typename list<T>::iterator iter = m_value_list.begin();

                for ( ; iter != m_value_list.end(); iter++ ) {
                    if ( *iter == value ) {
                        return iter;
                    }
                }

                return m_value_list.push_back ( value );
            }
        }
		bool test ( const T &value ) {
			SpinLock::Instance inst = m_lock.Lock();
			if ( !m_value_list.size() ) {
				return false;
			} else {
				typename list<T>::iterator iter = m_value_list.begin();

				for ( ; iter != m_value_list.end(); iter++ ) {
					if ( *iter == value ) {
						return true;
					}
				}

				return false;
			}
		}
		void clear( ) {
			SpinLock::Instance inst = m_lock.Lock();
			m_value_list.clear();
		}
		typename list<T>::iterator begin() {
			return m_value_list.begin();
		}
		typename list<T>::iterator end() {
			return m_value_list.end();
		}
    };
public:
    bucket m_buckets[HASH_BUCKET_COUNT];
    const T &insert ( const T &value ) {
        euint32 hash_value = _hash(value);
        euint32 key = hash_value & HASH_MASK;
        typename list<T>::iterator iter = m_buckets[key].insert ( value );
        return *iter;
    }
	bucket &get_bucket( euint32 _hash_value ) {
		euint32 key = _hash_value & HASH_MASK;
		return m_buckets[key];
	}
	bool test ( const T &value ) {
		euint32 hash_value = _hash(value);
		euint32 key = hash_value & HASH_MASK;
		return m_buckets[key].test ( value );
	}
	void clear( ) {
		for (int i = 0; i < HASH_BUCKET_COUNT; i++) {
			m_buckets[i].clear();
		}
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
