typedef struct MemPoolDef(mem_pool)
{
	euint real_chk_size;
	euint num_chunk_per_mem_block;
	List mem_pool_chain;
	struct MemPoolDef(mem_pool)* next;
#ifdef ALLOW_CONCURRENT
	ELock elock;
#endif
} MemPoolDef(mem_pool);

typedef struct MemPoolListDef(mem_pool_list)
{
	MemPoolDef(mem_pool)* head;
	MemPoolDef(mem_pool)* tail;
#ifdef ALLOW_CONCURRENT
	ELock elock;
#endif
} MemPoolListDef(mem_pool_list);

typedef struct MemDescDef(mem_desc)
{
	MemPoolDef(mem_pool)* mem_pool;
	Iterator iter;
} MemDescDef(mem_desc);

typedef struct AllocInfoDef(alloc_info)
{
	MemPoolDef(mem_pool)* mem_pool;
	Iterator iter;
} AllocInfoDef(alloc_info);

MemPoolDef(mem_pool)* MemPoolFunc(new)(euint _chk_size)
{
	MemPoolDef(mem_pool)* ret = NULL;
	euint num_chunk_per_mem_block = 0;
	euint c = 1;
	MemPoolNode node = {NULL};
	var v;

	ret = (struct MemPoolDef(mem_pool)*)malloc(sizeof(mem_pool));
	ret->mem_pool_chain = List_new(Vptr,  (MALLOC)malloc, (MFREE)free);
	ret->real_chk_size = cale_alloc_size(_chk_size);
	ret->next = NULL;

	while (!num_chunk_per_mem_block)
	{
		num_chunk_per_mem_block = (PAGE_SIZE * c) / cale_alloc_size(_chk_size);
		c++;
	}
	ret->num_chunk_per_mem_block = num_chunk_per_mem_block;

	node = MemPoolNode_new(_chk_size, ret->num_chunk_per_mem_block);

	v.vptr_var = node.self;
	List_push_back(ret->mem_pool_chain, v);

	ret->num_chunk_per_mem_block *= 2;
#ifdef ALLOW_CONCURRENT
	ELock_Init(&ret->elock);
#endif
	return ret;
}

void MemPoolFunc(delete)(MemPoolDef(mem_pool)* _self)
{
	Iterator iter = List_begin(_self->mem_pool_chain);
	while (iter)
	{
		var data = List_get_value(iter);
		MemPoolNode node = {(mem_pool_node*)data.vptr_var};
		MemPoolNode_delete(node);
		iter = List_next(iter);
	}
	free(_self);
}

totel_refer_info MemPoolFunc(log)(MemPoolDef(mem_pool)* _self)
{
	Iterator iter = List_begin(_self->mem_pool_chain);
	totel_refer_info ret;
	ret.unused_mem_size = 0;
	ret.used_mem_size = 0;

	while (iter)
	{
		var v = List_get_value(iter);
		mem_pool_node* node = (mem_pool_node*)v.vptr_var;
		totel_refer_info info = mem_pool_node_log(node);
		ret.unused_mem_size += info.unused_mem_size;
		ret.used_mem_size += info.used_mem_size;
		iter = List_next(iter);
	}
	return ret;
}

bool MemPoolFunc(check)(MemPoolDef(mem_pool)* _self)
{
	Iterator iter = List_begin(_self->mem_pool_chain);

	while (iter)
	{
		var v = List_get_value(iter);
		mem_pool_node* node = (mem_pool_node*)v.vptr_var;
		MemPoolNode n = {node};
		if (n.self->mem_list == (void*)0xffffffff)
			return false;
		if (!MemPoolNode_check(n))
			return false;
		iter = List_next(iter);
	}
	return true;
}

void* MemPoolFunc(alloc)(MemPoolDef(mem_pool)* _self,
						 Iterator* _iter,
						 bool _is_safe_alloc)
{
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_self->elock);
#endif
	Iterator iter = List_begin(_self->mem_pool_chain);
	var v = List_get_value(iter);
	MemPoolNode node = {(struct _mem_pool_node*)v.vptr_var};
	void* ret = MemPoolNode_alloc(node, _is_safe_alloc);
	*_iter = iter;
	if (MemPoolNode_empty(node))
	{
		List_throw_back(_self->mem_pool_chain, iter);
		iter = List_begin(_self->mem_pool_chain);
		v = List_get_value(iter);
		node.self = (struct _mem_pool_node*)v.vptr_var;
		if (MemPoolNode_empty(node))
		{
			euint chk_size = _self->real_chk_size;
			euint num_chks = _self->num_chunk_per_mem_block;
			node = MemPoolNode_new(chk_size, num_chks);
			v.vptr_var = node.self;
			List_push_front(_self->mem_pool_chain, v);
			_self->num_chunk_per_mem_block *= 2;
#ifdef ALLOW_CONCURRENT
			ELock_unlock(&_self->elock);
#endif
			return ret;
		}
	}
#ifdef ALLOW_CONCURRENT
	ELock_unlock(&_self->elock);
#endif
	return ret;
}

void MemPoolFunc(free)(MemPoolDef(mem_pool)* _self,
					   Iterator _iter,
					   void* _ptr)
{
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_self->elock);
#endif
	var v = List_get_value(_iter);
	MemPoolNode node = {(struct _mem_pool_node*)v.vptr_var};
	MemPoolNode_free(node, _ptr);
	List_throw_front(_self->mem_pool_chain, _iter);
#ifdef ALLOW_CONCURRENT
	ELock_unlock(&_self->elock);
#endif
}

euint MemPoolFunc(chunk_size)(MemPoolDef(mem_pool)* _self)
{
	return _self->real_chk_size;
}

MemPoolListDef(mem_pool_list)* MemPoolListFunc(new)()
{
	MemPoolListDef(mem_pool_list)* ret = NULL;
	ret = (struct MemPoolListDef(mem_pool_list)*)malloc(sizeof(mem_pool_list));
	ret->head = NULL;
	ret->tail = NULL;
#ifdef ALLOW_CONCURRENT
	ret->elock = 0;
#endif
	return ret;
}
void MemPoolListFunc(delete)(MemPoolListDef(mem_pool_list)* _self)
{
	free(_self);
}
MemPoolDef(mem_pool)* MemPoolListFunc(pop_front)(MemPoolListDef(mem_pool_list)* _self)
{
	MemPoolDef(mem_pool)* ret= NULL;
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_self->elock);
#endif
	if (_self->head) {
		ret = _self->head;
		_self->head = _self->head->next;
		if (!_self->head)
			_self->tail = NULL;
	}
#ifdef ALLOW_CONCURRENT
	ELock_unlock(&_self->elock);
#endif
	return ret;
}
void MemPoolListFunc(push_back)(MemPoolListDef(mem_pool_list)* _self, MemPoolDef(mem_pool)* _pool)
{
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_self->elock);
#endif
	_pool->next = NULL;
	if (_self->tail) {
		_self->tail->next = _pool;
		_self->tail = _pool;
	}
	else {
		_self->head = _self->tail = _pool;
	}
#ifdef ALLOW_CONCURRENT
	ELock_unlock(&_self->elock);
#endif
}

typedef struct MemAllocatorDef(mem_allocator)
{
	MemPoolDef(mem_pool)* mem_pools[MAX_MEM_POOLS];
#ifdef ALLOW_CONCURRENT
	ELock elock;
#endif
	euint32 test_mark;
	euint alloced_mem_size;
	mem_pool_node* head;
	mem_pool_node* tail;
} MemAllocatorDef(mem_allocator);

MemAllocatorDef(mem_allocator)* MemAllocatorFunc(new)()
{
    MemAllocatorDef(mem_allocator)* ret = NULL;
    int i = 0;
	ret = (struct MemAllocatorDef(mem_allocator)*)malloc(sizeof(MemAllocatorDef(mem_allocator)));
	memset(ret->mem_pools, 0, sizeof(ret->mem_pools));
    for (; i < MAX_MEM_POOLS; i++)
    {
        MemPoolDef(mem_pool)* mp =
        MemPoolFunc(new)( (i + 1) * DEFAULT_CHUNK_SIZE +
                    ALLOC_INFO_RESERVED + REFER_INFO_RESERVED );
        ret->mem_pools[i] = mp;
    }
#ifdef ALLOW_CONCURRENT
	ELock_Init(&ret->elock);
#endif
    ret->test_mark = (euint32)rand();
	ret->alloced_mem_size = 0;
	ret->head = NULL;
	ret->tail = NULL;
	return ret;
}
void MemAllocatorFunc(delete)(MemAllocatorDef(mem_allocator)* _self)
{
	int i = 0;
	for (; i < MAX_MEM_POOLS; i++)
	{
		if (_self->mem_pools[i]) 
		{
			MemPoolFunc(delete)(_self->mem_pools[i]);
		}
	}
	free(_self);
}
void* MemAllocatorFunc(alloc)(MemAllocatorDef(mem_allocator)* _self, euint _size, bool _is_safe_alloc)
{
	char* ret = NULL;
	AllocInfoDef(alloc_info) ainfo = {{NULL}, NULL};
	refer_info rinfo = {NULL, 0, _self->test_mark};
	euint i = 0;

	_size += (ALLOC_INFO_RESERVED + REFER_INFO_RESERVED);
	i = _size / DEFAULT_CHUNK_SIZE;

	if (i < MAX_MEM_POOLS)
	{
		MemPoolDef(mem_pool)* mp = _self->mem_pools[i];
		Iterator iter;
        ret = (char*)MemPoolFunc(alloc)(mp, &iter, _is_safe_alloc);
		ainfo.mem_pool = mp;
		ainfo.iter = iter;

		_self->alloced_mem_size += MemPoolFunc(chunk_size)(mp);
	}
	else
	{
#ifndef __APPLE__
		ret = (char*)__mingw_aligned_malloc(_size, 16);
#else
        ret = (char*)malloc(_size);
#endif
		if (_is_safe_alloc)
		    meminit(ret, _size);
	}
	((AllocInfoDef(alloc_info)*)ret)[0] = ainfo;
	ret += ALLOC_INFO_RESERVED;
	((refer_info*)ret)[0] = rinfo;
	ret += REFER_INFO_RESERVED;

	return ret;
}
void* MemAllocatorFunc(salloc)(MemAllocatorDef(mem_allocator)* _self, euint _size)
{
	return MemAllocatorFunc(alloc)(_self, _size, true);
}
void MemAllocatorFunc(free)(MemAllocatorDef(mem_allocator)* _self, void* _ptr)
{
	refer_info* ip = NULL;
	char* ptr = (char*)_ptr;
	AllocInfoDef(alloc_info) info = {{NULL}, NULL};

	if (!_ptr) {
		return;
	}
	ptr -= (ALLOC_INFO_RESERVED + REFER_INFO_RESERVED);
	info = ((AllocInfoDef(alloc_info)*)ptr)[0];

	ip = ((refer_info*)((char*)_ptr - REFER_INFO_RESERVED));
    
    EAssert( ip->test_mark == _self->test_mark, "mem error" );
	ip->file_name = NULL;
	ip->line = 0;
	ip->test_mark = 0;

	if ( !to_ptr(info.mem_pool) )
	{
#ifndef __APPLE__
		__mingw_aligned_free(ptr);
#else
        free(ptr);
#endif
	}
	else
	{
		MemPoolFunc(free)(info.mem_pool, info.iter, ptr);

		_self->alloced_mem_size -= MemPoolFunc(chunk_size)(info.mem_pool);
	}
}
void MemAllocatorFunc(log)(MemAllocatorDef(mem_allocator)* _self)
{
	euint i = 0;
	totel_refer_info ret;
	ret.unused_mem_size = 0;
	ret.used_mem_size = 0;
	for (; i < MAX_MEM_POOLS; i++)
	{
		if (_self->mem_pools[i]) 
		{
			totel_refer_info info = MemPoolFunc(log)(_self->mem_pools[i]);
			ret.unused_mem_size += info.unused_mem_size;
			ret.used_mem_size += info.used_mem_size;
		}
	}
#if BIT_WIDTH == 32
	elog("totel used mem %d totel unused mem %d", ret.used_mem_size, ret.unused_mem_size);
#elif BIT_WIDTH == 64
    elog("totel used mem %lld totel unused mem %lld", ret.used_mem_size, ret.unused_mem_size);
#endif
}
bool MemAllocatorFunc(check)(MemAllocatorDef(mem_allocator)* _self)
{
	euint i = 0;
	for (; i < MAX_MEM_POOLS; i++)
	{
		if (_self->mem_pools[i] && !MemPoolFunc(check)(_self->mem_pools[i]))
			return false;
	}
	return true;
}
euint MemAllocatorFunc(get_alloced_mem_size)(MemAllocatorDef(mem_allocator)* _self)
{
	return _self->alloced_mem_size;
}