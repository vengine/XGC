#include "pch.h"
#include "emem2.h"

_INLINE_ esint _get_aligned_16_size(esint size)
{
	esint num_qwords = size / 16;
	esint mod = size % 16;
	if ( mod ) {
		num_qwords++;
	}
	esint real_size = num_qwords * 16;
	return real_size;
}
_INLINE_ esint _get_head_size()
{
	return _get_aligned_16_size(sizeof(memory_node_head));
}
_INLINE_ vptr MemoryNodeHead_get_memory(MemoryNodeHead _self)
{
	esint real_size = _get_head_size();
    char* mem = (char*)_self;
	return (vptr)(mem + real_size);
}
_INLINE_ MemoryNodeHead MemoryNodeHead_get_node_head(vptr _ptr)
{
	esint real_size = _get_head_size();
	char* mem = (char*)_ptr;
	return (MemoryNodeHead)(mem - real_size);
}
_INLINE_ MemoryNodeHead MemoryNodeHead_Init(vptr mem, esint size)
{
	MemoryNodeHead ret = (MemoryNodeHead)mem;
	ret->totel_size = size;
	ret->used_size = 0;
	ret->catch_node = NULL;
	ret->prev_node_head = NULL;
	ret->next_node_head = NULL;
	return ret;
}
_INLINE_ esint MemoryNodeHead_get_available_size(MemoryNodeHead _self)
{
	return _self->totel_size - _self->used_size - _get_head_size();
}
vptr MemoryNodeHead_alloc(MemoryNodeHead _self, esint _size)
{
	esint real_size = _get_aligned_16_size(_size);
	if (MemoryNodeHead_get_available_size(_self) >= real_size)
	{
		esint half_available_size = _get_aligned_16_size(MemoryNodeHead_get_available_size(_self) / 2);
		esint new_totel_size = 0;
		if (half_available_size >= real_size + _get_head_size())
            new_totel_size = _self->totel_size - half_available_size;
		else
            new_totel_size = _self->totel_size - real_size - _get_head_size();
		char* mem = (char*)MemoryNodeHead_get_memory(_self);
		MemoryNodeHead next_node_head = (MemoryNodeHead)(mem + new_totel_size);
		MemoryNodeHead_Init(next_node_head, _self->totel_size - new_totel_size - _get_head_size());
		_self->totel_size = new_totel_size;
		MemoryNodeHead tmp = _self->next_node_head;
		_self->next_node_head = next_node_head;
		next_node_head->prev_node_head = _self;
		next_node_head->next_node_head = tmp;
		return MemoryNodeHead_get_memory(next_node_head);
	}
	else
	{
		if (_self->next_node_head)
			return MemoryNodeHead_alloc(_self->next_node_head, _size);
		else
			return NULL;
	}
}
void MemoryNodeHead_free(vptr _ptr)
{
    MemoryNodeHead node_head = MemoryNodeHead_get_node_head(_ptr);
	node_head->used_size = 0;
	/// rollback
	while (true) {
		if (node_head->prev_node_head && !node_head->prev_node_head->used_size) {
			node_head = node_head->prev_node_head;
		}
		else
			break;
	}
	/// try merge next memory node
	while (node_head->next_node_head) {
		if (node_head->next_node_head->used_size)
			break;
		node_head->totel_size += node_head->next_node_head->totel_size + _get_head_size();
		node_head->next_node_head = node_head->next_node_head->next_node_head;
	}
}
BigMemoryAlloctor BigMemoryAlloctor_new(esint _totel_size)
{
	BigMemoryAlloctor ret;
	esint real_totel_size = _get_aligned_16_size(_totel_size);
	ret = (BigMemoryAlloctor)malloc(sizeof(big_memory_allocator));
	ret->totel_mem_size = real_totel_size;
	ret->root = (MemoryNodeHead)malloc(_get_head_size() + real_totel_size);
	MemoryNodeHead_Init(ret->root, real_totel_size);
	return ret;
}
void BigMemoryAlloctor_delete(BigMemoryAlloctor _self)
{
	free(_self->root);
	free(_self);
}
vptr BigMemoryAlloctor_alloc(BigMemoryAlloctor _self, esint _size)
{
	return MemoryNodeHead_alloc(_self->root, _size);
}
void BigMemoryAlloctor_free(vptr _ptr)
{
	MemoryNodeHead_free(_ptr);
}