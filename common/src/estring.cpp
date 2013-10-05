/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "pch.h"
#include "estring.h"
#include "hash.h"
#include "emem.h"
#include "string.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4200)
#endif
typedef struct _estring
{
    euint32 length;
    char str[0];
} estring;
#ifdef _MSC_VER
#pragma warning(pop)
#endif
_INLINE_ estring* _get_estring(const char* _str)
{
    const char* ret = _str - sizeof(estring);
    return (estring*)ret;
}

_INLINE_ euint32 _string_length(const char* _str)
{
    euint32 count = 0;
    while(_str[count])
    {
        count++;
    }
    return count;
}
EString _EString_new(const char* _str, const char* _file_name, euint32 _file_line)
{
    euint32 length = _string_length(_str);
    estring* ret = (estring*)_Malloc(sizeof(estring) + length + 1, _file_name, (euint32)_file_line);

    ret->length = length;
    memcpy(&ret->str[0], _str, length + 1);
    return &ret->str[0];
}

void _EString_delete(EString _str, const char* _file_name, euint32 _file_line)
{
    estring* estr = _get_estring(_str);
    Mfree(estr);
}

EString _EString_add(EString _str0, const char* _str1, const char* _file_name, euint32 _file_line)
{
    estring* estr = _get_estring(_str0);
    euint32 length = (euint32)strlen(_str1);
    estring* ret = (estring*)_Malloc(sizeof(estring) + estr->length + length + 1, _file_name, (euint32)_file_line);

    char* dest = &ret->str[0];
    memcpy(dest, _str0, estr->length); dest += estr->length;
    memcpy(dest, _str1, length + 1);

    ret->length = estr->length + length;

    return &ret->str[0];
}

euint32 EString_size(EString _str)
{
    estring* estr = _get_estring(_str);
    return estr->length;
}

EString _EString_add_sint(EString _str, esint _si, const char* _file_name, euint32 _file_line)
{
    char mbuf[STRING_BUFFER_SIZE];
#if BIT_WIDTH == 32
    snprintf(mbuf, STRING_BUFFER_SIZE - 1, "%d", _si);
#elif BIT_WIDTH == 64
    snprintf(mbuf, STRING_BUFFER_SIZE - 1, "%lld", _si);
#endif
    return EString_add(_str, mbuf);
}

EString _EString_add_uint(EString _str, euint _ui, const char* _file_name, euint32 _file_line)
{
    char mbuf[STRING_BUFFER_SIZE];
#if BIT_WIDTH == 32
    snprintf(mbuf, STRING_BUFFER_SIZE - 1, "%d", _ui);
#elif BIT_WIDTH == 64
    snprintf(mbuf, STRING_BUFFER_SIZE - 1, "%lld", _ui);
#endif
    return EString_add(_str, mbuf);
}

EString _EString_add_float(EString _str, float _f, const char* _file_name, euint32 _file_line)
{
    char mbuf[STRING_BUFFER_SIZE];
    snprintf(mbuf, STRING_BUFFER_SIZE - 1, "%f", _f);
    return EString_add(_str, mbuf);
}

bool EString_less(EString _str0, EString _str1)
{
    estring* estr0 = (estring*)((ref_ptr)_str0 - (ref_ptr)sizeof(estring));
    estring* estr1 = (estring*)((ref_ptr)_str1 - (ref_ptr)sizeof(estring));
	return strcmp(estr0->str, estr1->str) < 0;
}

bool EString_greater(EString _str0, EString _str1)
{
    estring* estr0 = (estring*)((ref_ptr)_str0 - (ref_ptr)sizeof(estring));
    estring* estr1 = (estring*)((ref_ptr)_str1 - (ref_ptr)sizeof(estring));
	return strcmp(estr0->str, estr1->str) > 0;
}

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