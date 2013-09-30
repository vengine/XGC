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