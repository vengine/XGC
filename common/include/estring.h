/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef ESTRING_H
#define ESTRING_H
#include "common.h"
#include "etypes.h"

API_EXPORT EString _EString_new(const char* _str, const char* _file_name, euint32 _file_line);
API_EXPORT void _EString_delete(EString _str, const char* _file_name, euint32 _file_line);
API_EXPORT EString _EString_add(EString _str0, const char* _str1, const char* _file_name, euint32 _file_line);

API_EXPORT euint32 EString_size(EString _str);

API_EXPORT EString _EString_add_sint(EString _str, esint _si, const char* _file_name, euint32 _file_line);
API_EXPORT EString _EString_add_uint(EString _str, euint _ui, const char* _file_name, euint32 _file_line);
API_EXPORT EString _EString_add_float(EString _str, float _f, const char* _file_name, euint32 _file_line);

API_EXPORT bool EString_less(EString _str0, EString _str1);
API_EXPORT bool EString_greater(EString _str0, EString _str1);

#define EString_new(s) _EString_new(s, __FILE__, __LINE__)
#define EString_delete(s) _EString_delete(s, __FILE__, __LINE__)
#define EString_add(s0, s1) _EString_add(s0, s1, __FILE__, __LINE__)

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