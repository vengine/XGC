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