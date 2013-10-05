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
#include "xhn_string.hpp"
/**
#if defined(__APPLE__)
xhn::Utf8::Utf8(const wchar_t* wsz)
{
	// OS X uses 32-bit wchar
	const int bytes = wcslen(wsz) * sizeof(wchar_t);
	// comp_bLittleEndian is in the lib I use in order to detect PowerPC/Intel
	CFStringEncoding encoding = comp_bLittleEndian ? kCFStringEncodingUTF32LE
		: kCFStringEncodingUTF32BE;
	CFStringRef str = CFStringCreateWithBytesNoCopy(NULL, 
		(const UInt8*)wsz, bytes, 
		encoding, false, 
		kCFAllocatorNull);

	const int bytesUtf8 = CFStringGetMaximumSizeOfFileSystemRepresentation(str);
	///m_utf8 = new char[bytesUtf8];
	m_utf8.resize(bytesUtf8);
	CFStringGetFileSystemRepresentation(str, &m_utf8[0], bytesUtf8);
	CFRelease(str);
}	
#elif defined(_WIN32) || defined(_WIN64)
xhn::Utf8::Utf8(const wchar_t* wsz)
{
	const int bytesUtf8 = WideCharToMultiByte( CP_UTF8, 0, wsz, -1, NULL, 0, NULL, NULL );
	m_utf8.resize(bytesUtf8);
	WideCharToMultiByte(CP_UTF8, 0, wsz, -1, &m_utf8[0], bytesUtf8, NULL, NULL);

	vector< char, FGetCharRealSizeProc<char> > buf8;
    utf8fromwcs(wsz, buf8);
	xhn::string str = buf8;
    vector< wchar_t, FGetCharRealSizeProc<wchar_t> > buf16;
	utf8towcs((const euint8*)str.c_str(), buf16);
	xhn::wstring wstr = buf16;
	wprintf(wstr.c_str());
	printf("here\n");
}
#endif
**/
xhn::Utf8::Utf8(const wchar_t* wsz)
{
	vector< char, FGetCharRealSizeProc<char> > buf;
	utf8fromwcs(wsz, buf);
	m_utf8 = string(buf);
}

xhn::Unicode::Unicode(const char* sz)
{
    vector< wchar_t, FGetCharRealSizeProc<wchar_t> > buf;
	utf8towcs(sz, buf);
	m_unicode = wstring(buf);
}

bool xhn::Utf8::utf8fromwcs(const wchar_t* wcs, vector< char, FGetCharRealSizeProc<char> >& outbuf)
{
	const wchar_t *pc = wcs;
	euint num_errors = 0;
	///int i = 0;
	for(unsigned int c = *pc; c != 0 ; c = *(++pc)) 
	{
		if (c < (1 << 7)) 
		{
			outbuf.push_back(char(c));
		} 
		else if (c < (1 << 11)) 
		{
			outbuf.push_back(char((c >> 6) | 0xc0));
			outbuf.push_back(char((c & 0x3f) | 0x80));
		} 
		else if (c < (1 << 16)) 
		{
			outbuf.push_back(char((c >> 12) | 0xe0));
			outbuf.push_back(char(((c >> 6) & 0x3f) | 0x80));
			outbuf.push_back(char((c & 0x3f) | 0x80));
		} 
		else if (c < (1 << 21)) 
		{
			outbuf.push_back(char((c >> 18) | 0xf0));
			outbuf.push_back(char(((c >> 12) & 0x3f) | 0x80));
			outbuf.push_back(char(((c >> 6) & 0x3f) | 0x80));
			outbuf.push_back(char((c & 0x3f) | 0x80));
		}
		else 
			++num_errors;
	}
	return num_errors == 0;
}

bool xhn::Unicode::utf8towcs(const char *utf8, vector< wchar_t, FGetCharRealSizeProc<wchar_t> >& outbuf)
{
	if(!utf8) return true;
	euint count = 0;
	const char* pc = (const char*)utf8;
	const char* last = NULL;
	euint b;
	euint num_errors = 0;
	int i = 0;
	while (utf8[count]) {
		count++;
	}
	if (!count) return true;
	last = &utf8[count - 1];
	while (*pc) 
	{
		b = *pc++;

		if( !b ) break; // 0 - is eos in all utf encodings

		if ((b & 0x80) == 0)
		{
			// 1-byte sequence: 000000000xxxxxxx = 0xxxxxxx
			;
		} 
		else if ((b & 0xe0) == 0xc0) 
		{
			// 2-byte sequence: 00000yyyyyxxxxxx = 110yyyyy 10xxxxxx
			if(pc == last) { outbuf.push_back('?'); ++num_errors; break; }
			b = (b & 0x1f) << 6;
			b |= (*pc++ & 0x3f);
		} 
		else if ((b & 0xf0) == 0xe0) 
		{
			// 3-byte sequence: zzzzyyyyyyxxxxxx = 1110zzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 1) { outbuf.push_back('?'); ++num_errors; break; }

			b = (b & 0x0f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			if(b == 0xFEFF &&
				i == 0) // bom at start
				continue; // skip it
		} 
		else if ((b & 0xf8) == 0xf0) 
		{
			// 4-byte sequence: 11101110wwwwzzzzyy + 110111yyyyxxxxxx = 11110uuu 10uuzzzz 10yyyyyy 10xxxxxx
			if(pc >= last - 2) { outbuf.push_back('?'); break; }

			b = (b & 0x07) << 18;
			b |= (*pc++ & 0x3f) << 12;
			b |= (*pc++ & 0x3f) << 6;
			b |= (*pc++ & 0x3f);
			// b shall contain now full 21-bit unicode code point.
			assert((b & 0x1fffff) == b);
			if((b & 0x1fffff) != b)
			{
				outbuf.push_back('?');
				++num_errors;
				continue;
			}
			if( sizeof(wchar_t) == 16 ) // Seems like Windows, wchar_t is utf16 code units sequence there.
			{
				outbuf.push_back(wchar_t(0xd7c0 + (b >> 10)));
				outbuf.push_back(wchar_t(0xdc00 | (b & 0x3ff)));
			}
			else if( sizeof(wchar_t) >= 21 ) // wchar_t is full ucs-4 
			{
				outbuf.push_back(wchar_t(b));
			}
			else
			{
				assert(0); // what? wchar_t is single byte here?
			}
		} 
		else 
		{
			assert(0); //bad start for UTF-8 multi-byte sequence"
			++num_errors;
			b = '?';
		}
		outbuf.push_back(wchar_t(b));
	}
	return num_errors == 0;
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