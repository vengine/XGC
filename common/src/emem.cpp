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
#include "emem.h"
#include "elog.h"
#include "eassert.h"

///#define USE_C_MALLOC
#ifndef __APPLE__
#include <malloc.h>
#else
#include <sys/malloc.h>
#endif
///==================================================================================///
#include "list.h"
#include "spin_lock.h"

typedef struct _mem_node
{
    vptr next;
    char data[1];
} mem_node;

typedef struct _totel_refer_info
{
    euint used_mem_size;
    euint unused_mem_size;
} totel_refer_info;

#ifdef USE_SSE
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

static char align_char[32];
static __m128* zero_ptr = NULL;
#endif
_INLINE_ void meminit(void* _mem, euint _size)
{
#ifdef USE_SSE
	euint i = 0;

	euint cnt = _size / 16;
	__m128* m128_ptr = NULL;
    
    if (!zero_ptr)
    {
        ref_ptr offs = (ref_ptr)align_char % 16;
		offs = 16 - offs;
        zero_ptr = (__m128*)(align_char + offs);
        memset(zero_ptr, 0, sizeof(__m128));
    }

    m128_ptr = (__m128*)_mem;
    for (; i < cnt; i++)
        m128_ptr[i] = *zero_ptr;
#else
    memset(_mem, 0, _size);
#endif
}

_INLINE_ void mem_node_init(mem_node* _node)
{
    memset(_node->data, 0, ALLOC_INFO_RESERVED + REFER_INFO_RESERVED);
}

#define cale_alloc_size(s) ( s + (16 - (s % 16)) );

void* alloc_mem_list(euint _chk_size, euint _num_chks, euint* _real_chk_size, vptr* _begin, vptr* _end, mem_node** _head)
{
	char* ret = NULL;
	euint totel_size = 0;
    euint chk_size = 0;
	euint i = 0;

	chk_size = cale_alloc_size(_chk_size);
	totel_size = chk_size * _num_chks;

    *_real_chk_size = chk_size;
#ifndef __APPLE__
    ret = (char*)__mingw_aligned_malloc(totel_size, 16);
#else
    ret = (char*)malloc(totel_size);
#endif
    *_head = (mem_node*)ret;
    *_begin = ret;

    for (; i < _num_chks; i++)
    {
        mem_node* node = (mem_node*)&ret[i * chk_size];
        mem_node_init(node);

        if (i < _num_chks - 1)
            node->next = &ret[ (i + 1) * chk_size ];
        else
        {
            node->next = NULL;
            *_end = node;
        }
    }
    return ret;
}
void free_mem_list(void* _mem_list)
{
#ifndef __APPLE__
    __mingw_aligned_free(_mem_list);
#else
    free(_mem_list);
#endif
}

typedef struct _mem_pool_node
{
    void* mem_list;
    euint real_chk_size;
    vptr begin;
    vptr end;
    mem_node* head;
	euint num_chks;
	euint chk_cnt;
    struct _mem_pool_node* next;
	struct _mem_pool_node* prev;
} mem_pool_node;

mem_pool_node allco_mem_pool_node(euint _chk_size, euint _num_chks)
{
    mem_pool_node ret;
    ret.mem_list = alloc_mem_list(_chk_size, _num_chks, &ret.real_chk_size, &ret.begin, &ret.end, &ret.head);
	ret.num_chks = _num_chks;
	ret.chk_cnt = _num_chks;
	ret.next = NULL;
	ret.prev = NULL;
    return ret;
}

totel_refer_info mem_pool_node_log(mem_pool_node* _node)
{
	char* i = (char*)_node->begin;
    totel_refer_info ret;
    ret.unused_mem_size = 0;
    ret.used_mem_size = 0;
    for (; (vptr)i <= _node->end; i+=_node->real_chk_size)
    {
        refer_info* info = (refer_info*)(i + ALLOC_INFO_RESERVED);
        char* ptr = i + ALLOC_INFO_RESERVED + REFER_INFO_RESERVED;
        if (info->file_name)
        {
            ret.used_mem_size += _node->real_chk_size;
#if BIT_WIDTH == 32
            elog("##ptr %x file %s, line %d", (ref_ptr)ptr, info->file_name, info->line);
#elif BIT_WIDTH == 64
            elog("##ptr %llx file %s, line %d", (ref_ptr)ptr, info->file_name, info->line);
#endif
        }
        else
        {
            ret.unused_mem_size += _node->real_chk_size;
        }
    }
    return ret;
}

bool is_from(mem_pool_node* _node, void* _ptr)
{
    bool test0 = (_ptr >= _node->begin && _ptr <= _node->end);
    bool test1 = ( ((ref_ptr)_ptr - (ref_ptr)_node->begin) % _node->real_chk_size == 0 );

    ///printf("##test0 %d test1 %d##is from [begin %x end %x], ptr %x\n", test0, test1, _node->begin, _node->end, _ptr);
    return (test0 && test1);
}

void* alloc(mem_pool_node* _node, bool _is_safe_alloc)
{
	mem_node* ret = _node->head;
	if(ret)
	{
		EAssert(is_from(_node, ret), "invalid memory node!");
		if (_node->head->next)
		{
			EAssert(is_from(_node, ret->next), "invalid memory node!");
			_node->head = (mem_node*)ret->next;
		}
		else
		{
			_node->head = NULL;
		}
		if (_is_safe_alloc)
		    meminit(ret, _node->real_chk_size);
		_node->chk_cnt--;
	}
	return ret;
}

void dealloc(mem_pool_node* _node, void* _ptr)
{
    mem_node* n = NULL;
    EAssert(is_from(_node, _ptr), "invalid memory node!");
	///meminit(_ptr, _node->real_chk_size);
    n = (mem_node*)_ptr;
    if (_node->head)
    {
        EAssert(is_from(_node, _node->head), "invalid memory node!");
        n->next = _node->head;
    }
    else
        n->next = NULL;
    _node->head = n;
	_node->chk_cnt++;
}

MemPoolNode MemPoolNode_new(euint _chk_size, euint _num_chks)
{
    MemPoolNode ret;
    ret.self = (struct _mem_pool_node*)malloc(sizeof(mem_pool_node));
    *ret.self = allco_mem_pool_node(_chk_size, _num_chks);
    return ret;
}

void MemPoolNode_delete(MemPoolNode _self)
{
    free_mem_list(_self.self->mem_list);
    free(_self.self);
}

void* MemPoolNode_alloc(MemPoolNode _self, bool _is_safe_alloc)
{
    return alloc(_self.self, _is_safe_alloc);
}

bool MemPoolNode_free(MemPoolNode _self, void* _ptr)
{
    if (is_from(_self.self, _ptr))
    {
        dealloc(_self.self, _ptr);
        return true;
    }
    return false;
}

bool MemPoolNode_check(MemPoolNode _self)
{
    mem_node* node = _self.self->head;
    while (node)
    {
        if (!is_from(_self.self, node))
            return false;
        node = (mem_node*)node->next;
    }
    return true;
}

bool MemPoolNode_empty(MemPoolNode _self)
{
    return SELF.head == NULL;
}

#define MAX_MEM_POOLS      512
#define DEFAULT_CHUNK_SIZE 32

void* align_malloc_16(euint size)
{
#ifndef __APPLE__
	return __mingw_aligned_malloc(size, 16);
#else
	return malloc(size);
#endif
}

#define PAGE_SIZE (512)
#define MemPoolDef(x) x
#define MemPoolListDef(x) x
#define MemDescDef(x) x
#define AllocInfoDef(x) x
#define MemAllocatorDef(x) x
#define MemPoolFunc(x) MemPool_##x
#define MemPoolListFunc(x) MemPoolList_##x
#define MemAllocatorFunc(x) MemAllocator_##x
#define ALLOW_CONCURRENT

#include "emem_allocator.h"

#undef MemPoolDef
#undef MemPoolListDef
#undef MemDescDef
#undef AllocInfoDef
#undef MemAllocatorDef
#undef MemPoolFunc
#undef MemPoolListFunc
#undef MemAllocatorFunc
#undef ALLOW_CONCURRENT

#define MemPoolDef(x) Unlocked_##x
#define MemPoolListDef(x) Unlocked_##x
#define MemDescDef(x) Unlocked_##x
#define AllocInfoDef(x) Unlocked##x
#define MemAllocatorDef(x) Unlocked_##x
#define MemPoolFunc(x) UnlockedMemPool_##x
#define MemPoolListFunc(x) UnlockedMemPoolList_##x
#define MemAllocatorFunc(x) UnlockedMemAllocator_##x

#include "emem_allocator.h"

static mem_allocator* g_MemAllocator = NULL;

#if 0
#include <crtdbg.h>
/* 
* 自定义AllocHook函数
* nAllocType:操作类型:分配、重分配、释放(_HOOK_ALLOC、_HOOK_REALLOC、_HOOK_FREE)
*  pvData:在释放或重新分配的情况下,指向将释放块;在分配情况下，该指针为空
*  nSize: 要分配的大小
*  nBlockUse:块类型(_FREE_BLOCK、_NORMAL_BLOCK、_CRT_BLOCK、_IGNORE_BLOCK、_CLIENT_BLOCK)
*  lRequest:与它关联的顺序请求编号
*  szFileName:进行分配的文件名
*  nLine:进行分配的行号
*  return: 返回TRUE指示分配操作可以继续,返回FALSE指示分配操作应失败
*/
int MyAllocHook( int nAllocType, void *pvData,  size_t nSize, int nBlockUse, long lRequest,  const unsigned char * szFileName, int nLine )
{
	//_CRT_BLOCK 块是由 C 运行时库函数内部进行的内存分配,必须显式地忽略
	if( nBlockUse == _CRT_BLOCK  )
		return TRUE;

	if( nAllocType == _HOOK_ALLOC )
	{
		printf("here\n");
	}
	else if( nAllocType == _HOOK_REALLOC )
	{
		printf("here\n");
	}
	else if( nAllocType == _HOOK_FREE )
	{
		printf("here\n");
	}
	return TRUE;
}
#endif

void MInit()
{
#ifndef USE_C_MALLOC
	if (!g_MemAllocator) {
        g_MemAllocator = MemAllocator_new();
	} 
#endif
#if 0
	_CrtSetAllocHook( MyAllocHook );
#endif
}

vptr _Malloc(euint _size, const char* _file, euint32 _line)
{
	vptr ret = Ealloc(_size);
#ifndef USE_C_MALLOC
	refer_info info;
	info.file_name = _file;
	info.line = _line;
    info.test_mark = ETestMark();
	((refer_info*)((char*)ret - REFER_INFO_RESERVED))[0] = info;
#endif
	return ret;
}

vptr _SMalloc(euint _size, const char* _file, euint32 _line)
{
    vptr ret = SEalloc(_size);
#ifndef USE_C_MALLOC
    refer_info info;
    info.file_name = _file;
    info.line = _line;
	info.test_mark = ETestMark();
    ((refer_info*)((char*)ret - REFER_INFO_RESERVED))[0] = info;
#endif
    return ret;
}

void _Mfree(vptr _ptr, const char* _file, euint32 _line)
{
    Efree(_ptr);
#ifdef USE_C_MALLOC
    ///EAssert(_heapchk(), "free memory error!");
#endif
}

bool Mtest(vptr _ptr)
{
    return true;
}

void Massert(vptr _ptr, const char* _info)
{
}

void Mlog()
{
#ifndef USE_C_MALLOC
	/**
	euint i = 0;
    totel_refer_info ret;
    ret.unused_mem_size = 0;
    ret.used_mem_size = 0;
    for (; i < MAX_MEM_POOLS; i++)
    {
		if (g_mem_pools[i].self) 
		{
			totel_refer_info info = MemPool_log(g_mem_pools[i]);
			ret.unused_mem_size += info.unused_mem_size;
			ret.used_mem_size += info.used_mem_size;
		}
    }
    elog("totel used mem %d totel unused mem %d", ret.used_mem_size, ret.unused_mem_size);
	**/
	MemAllocator_log(g_MemAllocator);
#endif
}

bool MCheck()
{
#ifndef USE_C_MALLOC
	/**
	euint i = 0;
    for (; i < MAX_MEM_POOLS; i++)
    {
        if (g_mem_pools[i].self && !MemPool_check(g_mem_pools[i]))
            return false;
    }
    return true;
	**/
	return MemAllocator_check(g_MemAllocator);
#else
	return true;
#endif
}

euint32 ETestMark()
{
#ifndef USE_C_MALLOC
	return g_MemAllocator->test_mark;
#else
    return 0;
#endif
}

bool ETest(vptr ptr)
{
#ifndef USE_C_MALLOC
    refer_info info;
	info = ((refer_info*)((char*)ptr - REFER_INFO_RESERVED))[0];
    return info.test_mark == ETestMark();
#else
    return true;
#endif
}

vptr Ealloc(euint _size)
{
#ifndef USE_C_MALLOC
	if (!g_MemAllocator) {
	    g_MemAllocator = MemAllocator_new();
    }
#endif
#if 0
	_CrtSetAllocHook( MyAllocHook );
#endif
#ifdef USE_C_MALLOC
	vptr ret = align_malloc_16(_size);
	return ret;
#else
	return MemAllocator_alloc(g_MemAllocator, _size, false);
#endif
}
vptr SEalloc(euint _size)
{
#ifndef USE_C_MALLOC
	if (!g_MemAllocator) {
		g_MemAllocator = MemAllocator_new();
	}
#endif
#if 0
	_CrtSetAllocHook( MyAllocHook );
#endif
#ifdef USE_C_MALLOC
    vptr ret = align_malloc_16(_size);
    if (ret)
        memset(ret, 0, _size);
    return ret;
#else
	return MemAllocator_salloc(g_MemAllocator, _size);
#endif
}
#if defined(_WIN32) || defined(_WIN64)
#include "crtdbg.h"
#endif
void Efree(vptr _ptr)
{
#ifdef USE_C_MALLOC
#if defined(_WIN32) || defined(_WIN64)
    _aligned_free(_ptr);
#elif defined(__APPLE__)
    free(_ptr);
#endif
    return;
#else
	MemAllocator_free(g_MemAllocator, _ptr);
#endif
}
const char* Minfo(vptr _ptr, euint32* _line)
{
#ifdef USE_C_MALLOC
    return NULL;
#else
    char* ptr = (char*)_ptr;
	refer_info info = {NULL, 0};

    if (!_ptr)
        return NULL;
    ptr -= (ALLOC_INFO_RESERVED);
    info = ((refer_info*)ptr)[0];
    *_line = info.line;
    return info.file_name;
#endif
}

euint Msize(vptr _ptr)
{
#ifdef USE_C_MALLOC
    return 0;
#else
    char* ptr = (char*)_ptr;
    alloc_info info = {{NULL}, NULL};

    if (!_ptr)
        return 0;
    ptr -= (ALLOC_INFO_RESERVED + REFER_INFO_RESERVED);
    info = ((alloc_info*)ptr)[0];
    if (to_ptr(info.mem_pool))
        return MemPool_chunk_size(info.mem_pool);
    else
        return 0;
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