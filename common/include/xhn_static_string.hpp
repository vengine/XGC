/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef XHN_STATIC_STRING_H
#define XHN_STATIC_STRING_H
#include "common.h"
#include "etypes.h"
#include "xhn_hash_set.hpp"
#include "xhn_string.hpp"
#include "xhn_utility.hpp"
namespace xhn
{
class static_string
{
private:
    static hash_set<string> s_static_string_set;
private:
    const char *m_str;
public:
	static void dest();
    static_string ( const char *str ) {
		euint32 hash_value = calc_hashnr ( str, _strlen ( str ) );
		xhn::hash_set<string>::bucket& b = s_static_string_set.get_bucket(hash_value);
		{
			SpinLock::Instance inst = b.m_lock.Lock();
			xhn::list<xhn::string>::iterator iter = b.begin();
			xhn::list<xhn::string>::iterator end = b.begin();
			for (; iter != end; iter++) {
				if (*iter == str) {
					m_str = (*iter).c_str();
					return;
				}
			}
		}
		string value ( str );
		const string &v = s_static_string_set.insert ( value );
		m_str = v.c_str();
    }
    static_string ( const static_string& str ) {
        m_str = str.m_str;
    }
    static_string () {
        string value ( "" );
        const string &v = s_static_string_set.insert ( value );
        m_str = v.c_str();
    }
    const char *c_str() const {
        return m_str;
    }
    bool operator < ( const static_string &str ) const {
        return m_str < str.m_str;
    }
    bool operator > ( const static_string &str ) const {
        return m_str > str.m_str;
    }
    bool operator == ( const static_string &str ) const {
        return m_str == str.m_str;
    }
    bool operator != ( const static_string &str ) const {
        return m_str != str.m_str;
    }
    euint size() const {
        return strlen(m_str);
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