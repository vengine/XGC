/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef EXCEPTION_H
#define EXCEPTION_H
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>

extern jmp_buf s_jmp_buf;
extern const char* s_exce_info;
extern int s_exce_id;
#ifndef __cplusplus
#define TRY(i) \
signal(SIGSEGV, SIGSEGV_handle); \
{ if (setjmp(s_jmp_buf)) goto catch_exception##i; }

#define CATCH(i) \
if (0) catch_exception##i:
#else
#define TRY(i) try {
#define CATCH(i) } catch(...)
#endif
#define GET_EXCE \
( s_exce_info )

#define GET_EXCE_ID \
( s_exce_id )

extern void SIGSEGV_handle(int s);
extern void throw_exception(int id, const char* what);

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