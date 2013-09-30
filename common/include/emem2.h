#ifndef EMEM2_H
#define EMEM2_H
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "etypes.h"

typedef struct _memory_node_head
{
	esint totel_size;
	esint used_size;
	struct _memory_cache_node* catch_node;
	struct _memory_node_head* prev_node_head;
	struct _memory_node_head* next_node_head;
} memory_node_head;
typedef struct _memory_node_head* MemoryNodeHead;

typedef struct _memory_cache_node
{
	esint available_size;
	struct _memory_node_head* memory_node_head;
	struct _memory_cache_node* prev;
	struct _memory_cache_node* next;
} memory_cache_node;
typedef struct _memory_cache_node* MemoryCacheNode;

typedef struct _memory_cache
{
	MemoryCacheNode free_cache_node;
	MemoryCacheNode allocated_cache_node;
} memory_cache;
typedef struct _memory_cache* MemoryCache;

typedef struct _big_memory_allocator
{
    esint totel_mem_size;
	MemoryNodeHead root;
} big_memory_allocator;
typedef struct _big_memory_allocator* BigMemoryAlloctor;

API_EXPORT BigMemoryAlloctor BigMemoryAlloctor_new(esint _totel_size);
API_EXPORT void BigMemoryAlloctor_delete(BigMemoryAlloctor _self);
API_EXPORT vptr BigMemoryAlloctor_alloc(BigMemoryAlloctor _self, esint _size);
API_EXPORT void BigMemoryAlloctor_free(vptr _ptr);

#endif