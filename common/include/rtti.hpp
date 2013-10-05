/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef RTTI_HPP
#define RTTI_HPP
class RTTI
{
public:
	RTTI (const char* typeName, const RTTI* parentRTTI)
		: m_typeName(typeName)
		, m_parentRTTI(parentRTTI)
	{}
	inline const char* GetTypeName() const {return m_typeName;}
	inline const RTTI* GetParentRTTI() const {return m_parentRTTI;}
protected:
	const char* m_typeName;
	const RTTI* m_parentRTTI;
};

#define DeclareRootRTTI \
public: \
static const RTTI s_RTTI; \
virtual const RTTI* GetRTTI() const {return &s_RTTI;} \
bool IsKindOf(const RTTI* rtti) const \
{ \
	const RTTI* tmp = GetRTTI(); \
	while (tmp) \
    { \
	    if (tmp == rtti) return true; \
	    tmp = tmp->GetParentRTTI(); \
    } \
	return false; \
} \
template <typename T> \
bool IsKindOf() const \
{ \
    return IsKindOf(&T::s_RTTI); \
} \
template <typename T> \
T* DynamicCast() \
{ \
    if (IsKindOf(&T::s_RTTI)) return (T*)this; \
	else                      return 0; \
} \
template <typename T> \
const T* DynamicCast() const\
{ \
	if (IsKindOf(&T::s_RTTI)) return (const T*)this; \
	else                      return 0; \
} 

#define DeclareRTTI \
public: \
static const RTTI s_RTTI; \
virtual const RTTI* GetRTTI() const {return &s_RTTI;}

#define ImplementRootRTTI(rootclassname) \
const RTTI rootclassname::s_RTTI(#rootclassname, 0)

#define ImplementRTTI(classname, parentclassname) \
const RTTI classname::s_RTTI(#classname, &parentclassname::s_RTTI)

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