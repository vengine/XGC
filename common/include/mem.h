#ifndef MEM_H
#define MEM_H
#include <stdio.h>
#include <stdlib.h>
#include "etypes.h"
#include "common.h"

#define _MEM_BLOCK_HEAD_SIZE_ sizeof(uint) * 8

/// 必须确保所有分配出来的内存都是经过初始化的
API_EXPORT void TestInit();
API_EXPORT void MInit();

API_EXPORT vptr _Malloc(uint _size, const char* _file, euint32 _line);
API_EXPORT void _Mfree(vptr _ptr, const char* _file, euint32 _line);

void* TestAlloc(uint _size);
void TestFree(void* _ptr);

#define Malloc(s) _Malloc(s, __FILE__, __LINE__)
#define Mfree(p) _Mfree(p, __FILE__, __LINE__)

API_EXPORT void Mlog();
API_EXPORT bool MCheck();

API_EXPORT vptr Ealloc(uint _size);
API_EXPORT void Efree(vptr _ptr);
API_EXPORT const char* Minfo(vptr _ptr, uint* _line);

typedef struct _mem_pool_node_
{
    struct _mem_pool_node* self;
} MemPoolNode;
API_EXPORT MemPoolNode MemPoolNode_new(uint _chk_size, uint _num_chks);
API_EXPORT void MemPoolNode_delete(MemPoolNode _self);
API_EXPORT void* MemPoolNode_alloc(MemPoolNode _self);
API_EXPORT bool MemPoolNode_free(MemPoolNode _self, void* _ptr);
API_EXPORT bool MemPoolNode_empty(MemPoolNode _self);

typedef struct mem_pool_
{
    struct _mem_pool* self;
} MemPool;
API_EXPORT MemPool MemPool_new(uint _chk_size);
API_EXPORT void* MemPool_alloc(MemPool _self, Iterator* _iter);
API_EXPORT void MemPool_free(MemPool _self, Iterator _iter, void* _ptr);
API_EXPORT uint MemPool_chunk_size(MemPool _self);
#endif
