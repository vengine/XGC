/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_HASH_MAP_H
#define XHN_HASH_MAP_H
#include "common.h"
#include "etypes.h"
#include "xhn_utility.hpp"
#include "xhn_string.hpp"
#include "xhn_pair.hpp"
#include "xhn_list.hpp"
#include "xhn_lock.hpp"
#define HASH_BUCKET_COUNT (1<<15)
#define HASH_MASK         (HASH_BUCKET_COUNT - 1)
namespace xhn
{
	template <typename T>
	struct FHashProc
	{
		euint32 operator() (const T& v) {
			return calc_hashnr ( (const char*)v, sizeof(T) );
		}
	};
	template< typename K, typename V, typename HASH_PROC = FHashProc<K> >
	class hash_map : public RefObject
	{
	public:
		class bucket
		{
		public:
			list< pair<K, V> > m_value_list;
			RWLock m_lock;
		public:
			typename list< pair<K, V> >::iterator insert ( const K& key, const V &value ) {
				RWLock::Instance inst = m_lock.GetWriteLock();
				if ( !m_value_list.size() ) {
					return m_value_list.push_back ( make_pair(key, value) );
				} else {
					typename list< pair<K, V> >::iterator iter = m_value_list.begin();

					for ( ; iter != m_value_list.end(); iter++ ) {
						if ( iter->first == key ) {
							return iter;
						}
					}

					return m_value_list.push_back ( make_pair(key, value) );
				}
			}
			bool erase ( const K& key ) {
                RWLock::Instance inst = m_lock.GetWriteLock();
				if ( !m_value_list.size() ) {
					return false;
				} else {
					typename list< pair<K, V> >::iterator iter = m_value_list.begin();

					for ( ; iter != m_value_list.end(); iter++ ) {
						if ( iter->first == key ) {
							m_value_list.remove(iter);
							return true;
						}
					}

					return false;
				}
			}
			bool test ( const K &key ) {
				RWLock::Instance inst = m_lock.GetReadLock();
				if ( !m_value_list.size() ) {
					return false;
				} else {
					typename list< pair<K, V> >::iterator iter = m_value_list.begin();

					for ( ; iter != m_value_list.end(); iter++ ) {
						if ( iter->first == key ) {
							return true;
						}
					}

					return false;
				}
			}
			V* find( const K &key ) {
				RWLock::Instance inst = m_lock.GetReadLock();
				if ( !m_value_list.size() ) {
					return NULL;
				} else {
					typename list< pair<K, V> >::iterator iter = m_value_list.begin();

					for ( ; iter != m_value_list.end(); iter++ ) {
						if ( iter->first == key ) {
							return &iter->second;
						}
					}

					return NULL;
				}
			}
			RWLock::Instance get_write_lock() {
				return m_lock.GetWriteLock();
			}
			RWLock::Instance get_read_lock() {
				return m_lock.GetReadLock();
			}
			V* find_unlock( const K &key ) {
				if ( !m_value_list.size() ) {
					return NULL;
				} else {
					typename list< pair<K, V> >::iterator iter = m_value_list.begin();

					for ( ; iter != m_value_list.end(); iter++ ) {
						if ( iter->first == key ) {
							return &iter->second;
						}
					}

					return NULL;
				}
			}
		};
	public:
		bucket m_buckets[HASH_BUCKET_COUNT];
		HASH_PROC m_hash_proc;
		const pair<K, V> &insert ( const K &key, const V& value ) {
			euint hash_value = m_hash_proc (key);
			euint ukey = hash_value & HASH_MASK;
			typename list< pair<K, V> >::iterator iter = m_buckets[ukey].insert ( key, value );
			return *iter;
		}
		bool test ( const K &key ) {
			euint hash_value = m_hash_proc(key);
			euint ukey = hash_value & HASH_MASK;
			return m_buckets[ukey].test ( key );
		}
		V* find( const K &key ) {
			euint hash_value = m_hash_proc(key);
			euint ukey = hash_value & HASH_MASK;
			return m_buckets[ukey].find ( key );
		}
		bucket* find_bucket( const K &key ) {
			euint hash_value = m_hash_proc(key);
			euint ukey = hash_value & HASH_MASK;
			return &m_buckets[ukey];
		}
		bool erase( const K &key ) {
			euint hash_value = m_hash_proc(key);
			euint ukey = hash_value & HASH_MASK;
			return m_buckets[ukey].erase(key);
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