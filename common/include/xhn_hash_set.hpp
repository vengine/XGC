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
