/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef EMEM_HPP
#define EMEM_HPP
#include "emem.h"
#include "eassert.h"
#include "elog.h"
#include "xhn_exception.hpp"

class BannedAllocObject
{
public:
	//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize )
	{
		VEngineExce(InvalidMemoryAllocException, "this object cant new operator");
		return NULL;
	}
	void operator delete( void *p)
	{
		VEngineExce(InvalidMemoryFreeException, "this object cant delete operator");
	}
	//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize, const char* file,int line )
	{
		VEngineExce(InvalidMemoryAllocException, "this object cant new operator");
		return NULL;
	}

	void operator delete( void *p, const char* file,int line )
	{
		VEngineExce(InvalidMemoryAllocException, "this object cant delete operator");
	}
	//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize, void* ptr )
	{
		EAssert(((ref_ptr)ptr % 16) == 0, "new object error");
		return ptr;
	}

	void operator delete( void *p, void* ptr )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void* operator new[]( size_t nSize )
	{
		VEngineExce(InvalidMemoryAllocException, "this object cant new operator");
		return NULL;
	}

	void operator delete[]( void* ptr, size_t nSize )
	{
		VEngineExce(InvalidMemoryAllocException, "this object cant delete operator");
	}
};

class MemObject
{
public:
    MemObject() {}
    ~MemObject() {}
//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize )
    {
        return Malloc( nSize );
    }
	void operator delete( void *p)
	{
		Mfree(p);
	}
//////////////////////////////////////////////////////////////////////////
    void* operator new( size_t nSize, const char* file,int line )
    {
        return _Malloc( nSize, file, line );
    }

	void operator delete( void *p, const char* file,int line )
    {
        Mfree(p);
    }
//////////////////////////////////////////////////////////////////////////
	void* operator new( size_t nSize, void* ptr )
	{
		EAssert(((ref_ptr)ptr % 16) == 0, "new object error");
		return ptr;
	}

	void operator delete( void *p, void* ptr )
	{
	}
//////////////////////////////////////////////////////////////////////////
	void* operator new[]( size_t nSize )
	{
		return Malloc( nSize );
	}

	void operator delete[]( void* ptr, size_t nSize )
	{
		Mfree(ptr);
	}
};

class RefObject : public MemObject
{
public:
	volatile esint32 ref_count;
	RefObject()
	{
		ref_count = 0;
	}
	~RefObject()
	{
	}
};

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