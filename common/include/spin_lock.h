#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H
#include "common.h"
#include "etypes.h"
typedef volatile esint32 ELock;
inline void ELock_Init(ELock* lock) {
	*lock = 0;
}
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
inline void ELock_lock(ELock* lock) {
	while (InterlockedCompareExchange((LONG volatile *)lock, (LONG)1, (LONG)0) == (LONG)1) 
	{}
}
inline void ELock_unlock(ELock* lock) {
	while (InterlockedCompareExchange((LONG volatile *)lock, (LONG)0, (LONG)1) == (LONG)0) 
	{}
}
#elif defined(__APPLE__)
inline void ELock_lock(ELock* lock) {
     while (!OSAtomicCompareAndSwap32((int32_t)0, (int32_t)1, (volatile int32_t*)lock))
     {}
}
inline void ELock_unlock(ELock* lock) {
    while (!OSAtomicCompareAndSwap32((int32_t)1, (int32_t)0, (volatile int32_t*)lock))
    {}
}
#endif
#endif