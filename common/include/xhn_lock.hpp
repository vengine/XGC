/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_LOCK_HPP
#define XHN_LOCK_HPP
#include "common.h"
#include "etypes.h"
#include "emem.hpp"
#include "xhn_smart_ptr.hpp"
#include "spin_lock.h"
namespace xhn
{
class MutexLock : public RefObject
{
private:
    pthread_mutex_t m_lock;
public:
	class Instance
	{
		friend class MutexLock;
	private:
		pthread_mutex_t* m_prototype;
		inline Instance(pthread_mutex_t* lock)
        : m_prototype(lock)
		{}
	public:
		inline Instance(const Instance& inst)
        : m_prototype(inst.m_prototype)
		{}
		inline ~Instance()
		{
			pthread_mutex_unlock(m_prototype);
		}
	};
public:
	inline MutexLock()
	{
		pthread_mutex_init(&m_lock, NULL);
	}
	inline ~MutexLock()
	{
		pthread_mutex_destroy(&m_lock);
	}
	inline Instance Lock()
	{
		pthread_mutex_lock(&m_lock);
		return Instance(&m_lock);
	}
};
class RWLock : public RefObject
{
private:
	pthread_rwlock_t m_lock;
	vptr m_userdata;
public:
	class Instance
	{
		friend class RWLock;
	private:
		pthread_rwlock_t* m_prototype;
		inline Instance(pthread_rwlock_t* lock)
			: m_prototype(lock)
		{}
	public:
		inline Instance(const Instance& inst)
			: m_prototype(inst.m_prototype)
		{}
		inline ~Instance()
		{
			pthread_rwlock_unlock(m_prototype);
		}
	};
public:
	inline RWLock()
	{
		pthread_rwlock_init(&m_lock, NULL);
		m_userdata = NULL;
	}
	inline ~RWLock()
	{
		pthread_rwlock_destroy(&m_lock);
	}
	inline Instance GetWriteLock()
	{
		pthread_rwlock_wrlock(&m_lock);
		return Instance(&m_lock);
	}
	inline Instance GetReadLock()
	{
		pthread_rwlock_rdlock(&m_lock);
		return Instance(&m_lock);
	}
	inline vptr GetUserdata() const {
		return m_userdata;
	}
	inline void SetUserdata(vptr userdata) {
		m_userdata = userdata;
	}
};
typedef SmartPtr<RWLock> RWLockPtr;

class RWLock2
{
private:
    pthread_rwlock_t m_lock;
public:
	class Instance
	{
		friend class RWLock2;
	private:
		pthread_rwlock_t* m_prototype;
		inline Instance(pthread_rwlock_t* lock)
			: m_prototype(lock)
		{}
	public:
        inline Instance(const Instance& inst)
			: m_prototype(inst.m_prototype)
		{}
		inline ~Instance()
		{
			pthread_rwlock_unlock(m_prototype);
		}
	};
public:
    inline RWLock2()
	{
		pthread_rwlock_init(&m_lock, NULL);
	}
	inline ~RWLock2()
	{
		pthread_rwlock_destroy(&m_lock);
	}
	inline Instance GetWriteLock()
	{
		pthread_rwlock_wrlock(&m_lock);
        return Instance(&m_lock);
	}
	inline Instance GetReadLock()
	{
		pthread_rwlock_rdlock(&m_lock);
		return Instance(&m_lock);
	}
};

class SpinLock : public RefObject
{
private:
	ELock m_lock;
	vptr m_userdata;
public:
	class Instance
	{
		friend class SpinLock;
	private:
		ELock* m_prototype;
		inline Instance(ELock* lock)
			: m_prototype(lock)
		{}
	public:
		inline Instance(const Instance& inst)
			: m_prototype(inst.m_prototype)
		{}
		inline ~Instance()
		{
			ELock_unlock(m_prototype);
		}
	};
public:
	inline SpinLock()
		: m_lock(0)
		, m_userdata(NULL)
	{}
	inline Instance Lock() 
	{
        ELock_lock(&m_lock);
		return Instance(&m_lock);
	}
	inline vptr GetUserdata() const {
		return m_userdata;
	}
	inline void SetUserdata(vptr userdata) {
		m_userdata = userdata;
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