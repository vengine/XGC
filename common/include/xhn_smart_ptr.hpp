/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef SMART_PTR_H
#define SMART_PTR_H
#include "common.h"
#include "etypes.h"
#include "xhn_vector.hpp"
#include "xhn_atomic_operation.hpp"
namespace xhn
{
template <typename T>
struct FDestCallbackProc
{
	inline void operator() (T* ptr) {}
};

template <typename T>
struct FGarbageCollectProc
{
    vector<T*> garbagePool;
	void operator() (T* garbage) {
		garbagePool.push_back(garbage);
	}
	~FGarbageCollectProc() {
		for (euint i = 0; i < garbagePool.size(); i++) {
			delete garbagePool[i];
		}
	}
};

template< typename T, typename DEST_CALLBACK = FDestCallbackProc<T>, typename GARBAGE_COLLECTOR = FGarbageCollectProc<T> >
class SmartPtr
{
public:
	class CheckoutHandle
	{
		friend class SmartPtr;
	private:
		T* m_cloned;
		T* m_original;
	private:
		CheckoutHandle(T* c, T* o)
			: m_cloned(c)
			, m_original(o)
		{
		}
	public:
        CheckoutHandle()
			: m_cloned(NULL)
			, m_original(NULL)
		{}
        CheckoutHandle(const CheckoutHandle& ch)
			: m_cloned(ch.m_cloned)
		    , m_original(ch.m_original)
		{
		}
		~CheckoutHandle()
		{
		}
		void operator= (const CheckoutHandle& ch)
		{
			m_cloned = ch.m_cloned;
			m_original = ch.m_original;
		}
		inline T* Get() {
			return m_cloned;
		}
	};
private:
	T* m_ptr;
	DEST_CALLBACK m_dest_callback;
public:
	inline void _inc(T* ptr)
	{
		if (ptr)
		{
			AtomicIncrement(&ptr->ref_count);
		}
	}
	inline void _dec(T* ptr)
	{
		T* must_deleted = NULL;
		if (ptr) {
			AtomicDecrement(&ptr->ref_count);
			if (!ptr->ref_count)
				must_deleted = ptr;
		}
		if (must_deleted)
		{
			m_dest_callback(m_ptr);
			delete m_ptr;
		}
	}
	SmartPtr()
		: m_ptr(NULL)
	{}
	SmartPtr(const SmartPtr& ptr)
	{
		_inc(ptr.m_ptr);
		m_ptr = ptr.m_ptr;
	}
	SmartPtr(T* ptr)
	{
		_inc(ptr);
		m_ptr = ptr;
	}
	~SmartPtr()
	{
		_dec(m_ptr);
		m_ptr = NULL;
	}
	T* operator = (T* ptr)
	{
		_inc(ptr);
		_dec(m_ptr);
		m_ptr = ptr;
		return m_ptr;
	}
	T* operator = (const SmartPtr ptr)
	{
		_inc(ptr.m_ptr);
		_dec(m_ptr);
		m_ptr = ptr.m_ptr;
		return m_ptr;
	}
	bool operator < (const SmartPtr& ptr) const
	{
		return m_ptr < ptr.m_ptr;
	}
	bool operator == (const SmartPtr& ptr) const
	{
		return m_ptr == ptr.m_ptr;
	}
	bool operator != (const SmartPtr& ptr) const
	{
		return m_ptr != ptr.m_ptr;
	}
	T* get()
	{
		return m_ptr;
	}
	const T* get() const
	{
		return m_ptr;
	}
	T* operator ->() {
		return m_ptr;
	}
	const T* operator ->() const {
		return m_ptr;
	}
	bool operator!() const {
		return !m_ptr;
	}
    operator bool () const {
        return m_ptr != NULL;
    }

	CheckoutHandle Checkout()
	{
		CheckoutHandle ret(m_ptr->clone(), m_ptr);
		return ret;
	}

	bool Submit(CheckoutHandle& src, GARBAGE_COLLECTOR& gc)
	{
#ifdef _WIN32
		if (InterlockedCompareExchange((LONG volatile *)&m_ptr, (LONG)src.m_cloned, (LONG)src.m_original) == (LONG)src.m_original) {
#else
        if (OSAtomicCompareAndSwapPtr((void*)src.m_original, (void*)src.m_cloned, (void* volatile *)&m_ptr)) {
#endif
			if (src.m_cloned) {
				AtomicIncrement(&src.m_cloned->ref_count);
			}
			if (src.m_original) {
				AtomicDecrement(&src.m_cloned->ref_count);
				if (!src.m_original->ref_count) {
					gc(src.m_original);
					src.m_original = NULL;
				}
			}
			return true;
		}
		else {
			if (src.m_cloned) {
				gc(src.m_cloned);
				src.m_cloned = NULL;
			}
			return false;
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
