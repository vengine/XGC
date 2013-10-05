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
#include "hash.h"
euint32 ELFHash( char   * str)
{
    euint32  hash  =   0 ;
    euint32  x     =   0 ;

    while  ( * str)
    {
        hash  =  (hash  <<   4 )  +  ( * str ++ );
        if  ((x  =  hash  &   0xF0000000L )  !=   0 )
        {
            hash  ^=  (x  >>   24 );
            hash  &=   ~ x;
        }
    }

    return  (hash  &   0x7FFFFFFF );
}

euint32 calc_hashnr(const char *key, euint length)
{
	if (!length)
		return 0;
    register euint32 nr=1, nr2=4;
    while (length--)
    {
        nr^= (((nr & 63)+nr2)*((euint32) (euint8) *key++))+ (nr << 8);
        nr2+=3;
    }
    return((euint32) nr);
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