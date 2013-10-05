/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef ELOG_H
#define ELOG_H
#include "common.h"
#include "etypes.h"
#ifndef __APPLE__
#include <pthread.h>
#else
#define _SCHED_H
#include </usr/include/sched.h>
#include </usr/include/pthread.h>
#endif
extern pthread_rwlock_t g_lock;
#define ELOG_BUFFER_SIZE 4096 * 2
API_EXPORT void ELog_Init();
API_EXPORT void ELog_Release();
extern char g_elog_buffer[ELOG_BUFFER_SIZE];
API_EXPORT void ELog_write();

#define elog(fmt,...) { pthread_rwlock_wrlock(&g_lock); snprintf(g_elog_buffer,ELOG_BUFFER_SIZE - 2,fmt,##__VA_ARGS__); ELog_write(); pthread_rwlock_unlock(&g_lock); }
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