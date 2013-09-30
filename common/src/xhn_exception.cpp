//
//  xhn_exception.cpp
//  ecg
//
//  Created by 徐 海宁 on 13-5-3.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//
#include "pch.h"
#include "xhn_exception.hpp"
#include "xhn_string.hpp"

class ExceptionBody : public MemObject
{
public:
	const char* m_file;
	euint32 m_line;
	xhn::string m_msg;
	xhn::string m_what;
public:
	ExceptionBody(const char* file, euint32 line, const char* msg)
		: m_file(file)
		, m_line(line)
		, m_msg(msg)
	{
	}
};
Exception::Exception(const char* file, euint32 line, const char* msg) {
    m_body = ENEW ExceptionBody(file, line, msg);
	elog("error: %s", what());
}
Exception::~Exception() {
    delete m_body;
}
const char* Exception::what() {
	const RTTI* rtti = GetRTTI();

	m_body->m_what = "VEngineError:";
	m_body->m_what += rtti->GetTypeName();
	m_body->m_what += m_body->m_msg;

	return m_body->m_what.c_str();
}

ImplementRootRTTI(Exception);
ImplementRTTI(FunctionException, Exception);
ImplementRTTI(FunctionArgumentException, FunctionException);
ImplementRTTI(FunctionExecutionException, FunctionException);
ImplementRTTI(UndesiredBranch, FunctionExecutionException);
ImplementRTTI(FunctionResultException, FunctionException);
ImplementRTTI(ObjectException, Exception);
ImplementRTTI(ObjectNameAlreadyExistedException, ObjectException);
ImplementRTTI(ObjectUninitializedException, ObjectException);
ImplementRTTI(InvalidEnumerationException, FunctionExecutionException);
ImplementRTTI(MemoryException, Exception);
ImplementRTTI(InvalidMemoryAllocException, MemoryException);
ImplementRTTI(InvalidMemoryFreeException, MemoryException);