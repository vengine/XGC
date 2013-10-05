/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

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