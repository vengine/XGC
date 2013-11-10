/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

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
inline euint32 AtomicDecrement(volatile esint32* i)
{
	return InterlockedDecrement((volatile LONG*)i);
}
#elif defined (__APPLE__)
inline void AtomicIncrement(volatile esint32* i)
{
	OSAtomicIncrement32(i);
}
inline euint32 AtomicDecrement(volatile esint32* i)
{
	return OSAtomicDecrement32(i);
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