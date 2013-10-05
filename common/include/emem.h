/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef EMEM_H
#define EMEM_H
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "etypes.h"

typedef struct _refer_info
{
	const char* file_name;
	euint32 line;
    euint32 test_mark;
} refer_info;
#define ALLOC_INFO_RESERVED 16
#define REFER_INFO_RESERVED 16

#define _MEM_BLOCK_HEAD_SIZE_ sizeof(euint) * 8

/// 必须确保所有分配出来的内存都是经过初始化的
API_EXPORT void TestInit();
/// 必须在启动多线程系统前完成引导
API_EXPORT void MInit();

API_EXPORT vptr _Malloc(euint _size, const char* _file, euint32 _line);
API_EXPORT vptr _SMalloc(euint _size, const char* _file, euint32 _line);
API_EXPORT void _Mfree(vptr _ptr, const char* _file, euint32 _line);

void* TestAlloc(euint _size);
void TestFree(void* _ptr);

#define Malloc(s) _Malloc(s, __FILE__, __LINE__)
#define SMalloc(s) _SMalloc(s, __FILE__, __LINE__)
#define Mfree(p) _Mfree(p, __FILE__, __LINE__)

API_EXPORT void Mlog();
API_EXPORT bool MCheck();

API_EXPORT euint32 ETestMark();
API_EXPORT bool ETest(vptr ptr);
API_EXPORT vptr Ealloc(euint _size);
API_EXPORT vptr SEalloc(euint _size);
API_EXPORT void Efree(vptr _ptr);
API_EXPORT const char* Minfo(vptr _ptr, euint32* _line);
API_EXPORT euint Msize(vptr _ptr);

typedef struct _mem_pool_node_
{
    struct _mem_pool_node* self;
} MemPoolNode;
API_EXPORT MemPoolNode MemPoolNode_new(euint _chk_size, euint _num_chks);
API_EXPORT void MemPoolNode_delete(MemPoolNode _self);
API_EXPORT void* MemPoolNode_alloc(MemPoolNode _self, bool _is_safe_alloc);
///API_EXPORT void* MemPoolNode_salloc(MemPoolNode _self);
API_EXPORT bool MemPoolNode_free(MemPoolNode _self, void* _ptr);
API_EXPORT bool MemPoolNode_empty(MemPoolNode _self);

typedef struct mem_pool_
{
    struct _mem_pool* self;
} MemPool;
API_EXPORT MemPool MemPool_new(euint _chk_size);
API_EXPORT void MemPool_delete(MemPool _self);
API_EXPORT void* MemPool_alloc(MemPool _self,
                               Iterator* _iter,
                               bool _is_safe_alloc);
API_EXPORT void* MemPool_salloc(MemPool _self, Iterator* _iter);
API_EXPORT void MemPool_free(MemPool _self,
                             Iterator _iter,
                             void* _ptr);
API_EXPORT euint MemPool_chunk_size(MemPool _self);

typedef struct mem_pool_list_
{
    struct _mem_pool_list* self;
} MemPoolList;

API_EXPORT MemPoolList MemPoolList_new();
API_EXPORT void MemPoolList_delete(MemPoolList _self);
API_EXPORT MemPool MemPoolList_pop_front(MemPoolList _self);
API_EXPORT void MemPoolList_push_back(MemPoolList _self, MemPool _pool);

typedef struct mem_allocator_
{
	struct _mem_allocator* self;
} MemAllocator;

API_EXPORT MemAllocator MemAllocator_new();
API_EXPORT void MemAllocator_delete(MemAllocator _self);
API_EXPORT void* MemAllocator_alloc(MemAllocator _self, euint _size, bool _is_safe_alloc);
API_EXPORT void* MemAllocator_salloc(MemAllocator _self, euint _size);
API_EXPORT void MemAllocator_free(MemAllocator _self, void* _ptr);
API_EXPORT void MemAllocator_log(MemAllocator _self);
API_EXPORT bool MemAllocator_check(MemAllocator _self);
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
