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
#include "elog.h"
#include "string.h"

pthread_rwlock_t g_lock;
char g_elog_buffer[ELOG_BUFFER_SIZE];
#ifdef USE_LOG_SYSTEM
static FILE* g_elog_file = NULL;
#endif
void ELog_Init()
{
#ifdef USE_LOG_SYSTEM
#if defined(_WIN32) || defined(_WIN64)
	g_elog_file = SafeFOpen("log.log", "w+");
#elif defined(__APPLE__)
    g_elog_file = SafeFOpen("/Users/joumining/vengine/log.log", "w+");
#endif
	pthread_rwlock_init(&g_lock, NULL);
#endif
}

void ELog_write()
{
#ifdef USE_LOG_SYSTEM
	///pthread_spin_lock(&g_lock);
    snprintf(g_elog_buffer, ELOG_BUFFER_SIZE - 1, "%s\n", g_elog_buffer);
    fwrite(g_elog_buffer, strlen(g_elog_buffer), 1, g_elog_file);
	///pthread_spin_unlock(&g_lock);
#endif
}

void ELog_Release()
{
#ifdef USE_LOG_SYSTEM
	///pthread_spin_lock(&g_lock);
    fclose(g_elog_file);
    g_elog_file = NULL;
	///pthread_spin_unlock(&g_lock);
#endif
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
