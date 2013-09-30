#ifndef XHN_ATOMIC_OPERATION_HPP
#define XHN_ATOMIC_OPERATION_HPP
#include "common.h"
#include "etypes.h"
#if defined (_WIN32) || defined (_WIN64)
#include <windows.h>
inline void AtomicIncrement(volatile esint32* i)
{
	InterlockedIncrement((volatile LONG*)i);
}
inline void AtomicDecrement(volatile esint32* i)
{
	InterlockedDecrement((volatile LONG*)i);
}
#elif defined (__APPLE__)
inline void AtomicIncrement(volatile esint32* i)
{
	OSAtomicIncrement32(i);
}
inline void AtomicDecrement(volatile esint32* i)
{
	OSAtomicDecrement32(i);
}
#endif

#ifdef _WIN32
/**
struct SpinLock
{
    volatile euint32 lock;
    SpinLock()
		: lock(0)
	{}
	inline void Init() {
		lock = 0;
	}
	inline void Lock() {
		while (InterlockedCompareExchange((LONG volatile *)&lock, (LONG)1, (LONG)0) == (LONG)1) 
		{}
	}
	inline void Unlock() {
		while (InterlockedCompareExchange((LONG volatile *)&lock, (LONG)0, (LONG)1) == (LONG)0) 
		{}
	}
};
**/
#endif
#endif