/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifdef _MSC_VER
#pragma warning(disable:4819)
#endif

#ifndef _COMMON_H_
#define _COMMON_H_
#define VPRINT elog
typedef void* Iterator;

/**
#ifdef  __GNUC__
#if __GNUC__ >= 3 // GCC3.0

#ifdef  _MSC_VER
#if _MSC_VER >=1000 // VC++4.0
#if _MSC_VER >=1100 // VC++5.0
#if _MSC_VER >=1200 // VC++6.0
#if _MSC_VER >=1300 // VC2003
#if _MSC_VER >=1400 // VC2005

#ifdef  __BORLANDC__

UNIX
#ifdef  __unix
or
#ifdef  __unix__

#ifdef  __linux
or
#ifdef  __linux__

#ifdef  __FreeBSD__
#ifdef  __NetBSD__

#ifdef  _WIN32
#ifdef  _WIN64

#ifdef  _WINDOWS
#ifdef  _CONSOLE

#if (WINVER >= 0x030a) // Windows 3.1
#if (WINVER >= 0x0400) // Windows 95/NT 4.0
#if (WINVER >= 0x0410) // Windows 98
#if (WINVER >= 0x0500) // Windows Me/2000
#if (WINVER >= 0x0501) // Windows XP
#if (WINVER >= 0x0600) // Windows Vista

#ifdef  _WIN32_WINDOWS
#if (_WIN32_WINDOWS >= 0x0400) // Windows 95
#if (_WIN32_WINDOWS >= 0x0410) // Windows 98
#if (_WIN32_WINDOWS >= 0x0500) // Windows Me

#if (_WIN32_WINNT  >= 0x0500) // Windows 2000
#if (_WIN32_WINNT  >= 0x0501) // Windows XP
#if (_WIN32_WINNT  >= 0x0600) // Windows Vista

#ifdef  _WIN32_WCE

#ifdef  __CYGWIN__

#ifdef  __CYGWIN32__

#ifdef  __MINGW32__
**/

#    ifdef __GNUC__
#        ifndef unlikely
#            define unlikely(x) __builtin_expect(!!(x), 0)
#        endif
#        ifndef likely
#            define likely(x) __builtin_expect(!!(x), 1)
#        endif
#    else
#        define unlikely(x) x
#        define likely(x) x
#    endif

#    define to_ptr(o) *((vptr*)&o)
#    define to_radian(a) a / 180.0 * 3.1415926535897932
#    define to_angle(r) r / 3.1415926535897932 * 180.0

#    if defined(__MINGW32__) || defined(__MINGW64__) || defined(__linux) || defined(__linux__)
#        define _INLINE_ static inline
#        define FORCE_ALIGN_ARG_POINTER __attribute__ ((force_align_arg_pointer))
#    elif defined(_MSC_VER)
#        define _INLINE_ static inline
#        define FORCE_ALIGN_ARG_POINTER
#        define snprintf sprintf_s
#    elif defined(__APPLE__)
#        define _INLINE_ inline
#        define FORCE_ALIGN_ARG_POINTER
#    endif

#    ifdef USE_OPENGL
#        define GLSL_MAIN_VERSION 1
#        define GLSL_SUB_VERSION  5

#        if defined(_WIN32) || defined(_WIN64)
#            include <windows.h>
#            include <GL/glew.h>
#        elif defined(__APPLE__)
#            if GLSL_MAIN_VERSION > 1 || GLSL_SUB_VERSION >= 5
#                include <OpenGL/gl3.h>
#            else
#                include <OpenGL/gl.h>
#            endif
#        endif
#    endif

#    define USE_SSE

#    define SELF (*_self.self)

#    ifdef DLL_EXPORT
#        define API_EXPORT __declspec(dllexport)
#    else
#        define API_EXPORT
#    endif

#    define MARK_GLSL_VERSION

#    define NEG_NAN 0xffc00000
#    define POS_NAN 0x7fc00000

#    if defined(__APPLE__)
#        define FLT_MIN __FLT_MIN__
#        define FLT_MAX __FLT_MAX__
#    else
#        include "float.h"
#    endif

#    ifdef __GNUC__
#        define SafeFOpen fopen
#    elif defined _MSC_VER
#        include <stdio.h>
#        include <stdlib.h>
        inline FILE* SafeFOpen(const char* path, const char* mode)
        {
	        FILE* ret = NULL;
	        fopen_s(&ret, path, mode);
	        return ret;
        }
#    endif

#    define ALIGN_FLAG(n) __declspec(align(n))

#    ifdef __cplusplus
#        define ENEW new(__FILE__, __LINE__)
#        define EDELETE delete(__FILE__, __LINE__)
#        define ENEW_ARRAY new
#        define EDELETE_ARRAY delete
#    endif

#    ifndef __APPLE__
#        include <omp.h>
#    else
#        include <libkern/OSAtomic.h>
#    endif

#    define USE_LOG_SYSTEM

#    ifdef _WIN32
#        include <pthread.h>
#        include <pthread_clock.h>
#        include <windows.h>
#        define BIT_WIDTH 32

#    elif defined (_WIN64)
#        include <pthread.h>
#        include <pthread_clock.h>
#        include <windows.h>
#        define BIT_WIDTH 64

#    elif defined(__APPLE__)
#        ifdef __i386__
#            define BIT_WIDTH 32
#        else
#            define BIT_WIDTH 64
#        endif
#    endif

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
