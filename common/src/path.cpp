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
#include "path.h"
#ifdef __MINGW32__
#include <dir.h>
#elif defined(_MSC_VER)
#include <io.h>
#include <string.h>
#endif
#ifndef __APPLE__
#include <direct.h>
#else
#include <stdio.h>
#include <sys/dir.h>
#include <sys/dirent.h>
#include "apple_file_manager.h"
#endif
#ifdef __APPLE__
class EFilenameArray : public FilenameArray
{
public:
    EStringArray* m_result;
public:
    EFilenameArray(EStringArray* strArray)
    : m_result(strArray)
    {}
    virtual void AddFilename(const char* filename);
};
void EFilenameArray::AddFilename(const char* filename)
{
    EString str = EString_new(filename);
    apush(*m_result, str);
}
#endif
void GetAllFileNamesInDir(const char* dir, const char* filter_rule, EStringArray* result)
{
#ifndef __APPLE__
    struct _finddata_t fd;
    array_n(*result) = 0;

    EString fulldir = EString_new(dir);
    euint size = EString_size(fulldir);
    if (size)
    {
        char end_char = fulldir[size - 1];
        if ('\\' != end_char && '/' != end_char)
        {
            EString tmp0 = EString_add(fulldir, "\\");
            EString tmp1 = EString_add(tmp0, filter_rule);
            EString_delete(fulldir);
            EString_delete(tmp0);
            fulldir = tmp1;
        }
        else
        {
            EString tmp0 = EString_add(fulldir, filter_rule);
            EString_delete(fulldir);
            fulldir = tmp0;
        }
    }

    long h = _findfirst(fulldir, &fd);
    int ret = 0;
    while (h != -1L && !ret)
    {
        if (strcmp(".", fd.name) != 0 && strcmp("..", fd.name) != 0)
        {
            EString filename = EString_new(fd.name);
            apush(*result, filename);
        }
        ret = _findnext(h, &fd);
    }
    _findclose(h);
    EString_delete(fulldir);
#else
    EFilenameArray filenameArray(result);
    GetFilenames(AppDirectory, &filenameArray);
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