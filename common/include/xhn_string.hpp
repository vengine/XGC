/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef XHN_STRING_HPP
#define XHN_STRING_HPP
#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "xhn_utility.hpp"
#include <string.h>
#include "xhn_string_base.hpp"

namespace xhn
{
struct FStrCmpProc {
    int operator() (const char* str0, const char* str1) const {
        return strcmp(str0, str1);
    }
};
struct FDefaultStrProc {
    const char* operator() () const {
        return "";
    }
};
typedef string_base<char, FStrCmpProc, FDefaultStrProc> string;

struct FWStrCmpProc {
    int operator() (const wchar_t* str0, const wchar_t* str1) const {
        int count = 0;
        while (str0[count]) {
            if (!str1[count])
                return 1;
            if (str0[count] < str1[count])
                return -1;
            else if (str0[count] > str1[count])
                return 1;
            count++;
        }
        if (str1[count])
            return -1;
        return 0;
    }
};
struct FDefaultWStrProc {
    const wchar_t* operator() () const {
        return L"";
    }
};
typedef string_base<wchar_t, FWStrCmpProc, FDefaultWStrProc> wstring;

class Utf8
{
private:
	string m_utf8;
private:
	bool utf8fromwcs(const wchar_t* wcs, vector< char, FGetCharRealSizeProc<char> >& outbuf);
public:
	Utf8(const wchar_t* wsz);
public:
	inline operator const string&() const { return m_utf8; }
};

class Unicode
{
private:
	wstring m_unicode;
private:
	bool utf8towcs(const char *utf8, vector< wchar_t, FGetCharRealSizeProc<wchar_t> >& outbuf);
public:
	Unicode(const char* sz);
public:
	inline operator const wstring&() const { return m_unicode; }
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
