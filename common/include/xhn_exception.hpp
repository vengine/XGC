//
//  xhn_exception.hpp
//  ecg
//
//  Created by 徐 海宁 on 13-5-3.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#ifndef ecg_xhn_exception_hpp
#define ecg_xhn_exception_hpp

#include <exception>
#include "rtti.hpp"
#include "elog.h"
class ExceptionBody;
class Exception : public std::exception
{
    DeclareRootRTTI;
public:
	ExceptionBody* m_body;
public:
    Exception(const char* file, euint32 line, const char* msg);
	~Exception();
    const char* what();
};
class FunctionException : public Exception
{
	DeclareRTTI;
public:
	FunctionException(const char* file, euint32 line, const char* msg)
		: Exception(file, line, msg)
	{}
};
/// 函数的输入参数为非法
class FunctionArgumentException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionArgumentException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};
/// 函数执行过程的异常
class FunctionExecutionException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionExecutionException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};
/// 进入不该进入的条件分支
class UndesiredBranch : public FunctionExecutionException
{
	DeclareRTTI;
public:
	UndesiredBranch(const char* file, euint32 line, const char* msg)
	: FunctionExecutionException(file, line, msg)
	{}
};
/// 函数结果不是期望值
class FunctionResultException : public FunctionException
{
    DeclareRTTI;
public:
    FunctionResultException(const char* file, euint32 line, const char* msg)
    : FunctionException(file, line, msg)
    {}
};

class ObjectException : public Exception
{
	DeclareRTTI;
public:
	ObjectException(const char* file, euint32 line, const char* msg)
		: Exception(file, line, msg)
	{}
};
    
/// switch case里的枚举值是无效的
class InvalidEnumerationException : public FunctionExecutionException
{
    DeclareRTTI;
public:
    InvalidEnumerationException(const char* file, euint32 line, const char* msg)
    : FunctionExecutionException(file, line, msg)
    {}
};
/// 对象名已存在
class ObjectNameAlreadyExistedException : public ObjectException
{
    DeclareRTTI;
public:
	ObjectNameAlreadyExistedException(const char* file, euint32 line, const char* msg)
		: ObjectException(file, line, msg)
	{}
};
class ObjectUninitializedException : public ObjectException
{
	DeclareRTTI;
public:
	ObjectUninitializedException(const char* file, euint32 line, const char* msg)
		: ObjectException(file, line, msg)
	{}
};
class MemoryException : public Exception
{
	DeclareRTTI
public:
	MemoryException(const char* file, euint32 line, const char* msg)
		: Exception(file, line, msg)
	{}
};
/// 无效内存分配异常
class InvalidMemoryAllocException : public MemoryException
{
    DeclareRTTI
public:
	InvalidMemoryAllocException(const char* file, euint32 line, const char* msg)
		: MemoryException(file, line, msg)
	{}
};
class InvalidMemoryFreeException : public MemoryException
{
	DeclareRTTI
public:
	InvalidMemoryFreeException(const char* file, euint32 line, const char* msg)
		: MemoryException(file, line, msg)
	{}
};
#define VEngineExce(e, m) throw e(__FILE__, __LINE__, (m))
#endif
