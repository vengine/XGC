#include "pch.h"
#include "golden_cudgel.h"
#include "emem.h"
#include <windows.h>
struct _golden_cudgel
{
    unsigned char* mem;
    unsigned char* tail;
    euint ele_size;
    euint ele_count;
};

#define MEMORY_PAGE_SIZE 4 * 1024
#define MEMORY_RESERVE_SIZE 128 * 1024 * 1024
///static unsigned char* mem_begin_addr = (unsigned char*)0x00000000;
GoldenCudgel GoldenCudgel_new(euint ele_size)
{
    if (MEMORY_PAGE_SIZE / ele_size < 16)
        return NULL;
    vptr mem = VirtualAlloc(NULL, MEMORY_RESERVE_SIZE, MEM_RESERVE, PAGE_READWRITE);
    if (mem)
    {
        ///mem_begin_addr += MEMORY_RESERVE_SIZE;
        GoldenCudgel ret = (GoldenCudgel)SMalloc(sizeof(struct _golden_cudgel));
        ret->mem = (unsigned char*)mem;
        ret->tail = (unsigned char*)mem;
        ret->ele_size = ele_size;
        ret->ele_count = 0;
        return ret;
    }
    else
        return NULL;
}
void GoldenCudgel_delete(GoldenCudgel self)
{
    VirtualFree(self->mem, 0, MEM_RELEASE);
    Mfree(self);
}

void GoldenCudgel_grow_up(GoldenCudgel self, vptr eles, euint ele_count)
{
    VirtualAlloc(self->tail, self->ele_size * ele_count, MEM_COMMIT, PAGE_READWRITE);
    memcpy(self->tail, eles, self->ele_size * ele_count);
    self->tail += self->ele_size * ele_count;
    self->ele_count += ele_count;
}

void GoldenCudgel_grow_up_not_fill(GoldenCudgel self, euint ele_count)
{
    VirtualAlloc(self->tail, self->ele_size * ele_count, MEM_COMMIT, PAGE_READWRITE);
    self->tail += self->ele_size * ele_count;
    self->ele_count += ele_count;
}

void GoldenCudgel_cut_down(GoldenCudgel self, euint ele_count)
{
    if (self->tail - self->ele_size * ele_count >= self->mem)
    {
        self->ele_count -= ele_count;
        self->tail = (unsigned char*)(self->ele_size * self->ele_count);
        VirtualFree(self->tail, self->ele_size * ele_count, MEM_DECOMMIT);
    }
}

vptr GoldenCudgel_get_element_pointer(GoldenCudgel self, euint i)
{
    if (i < self->ele_count)
    {
        return self->mem + self->ele_size * i;
    }
    else
    {
        return NULL;
    }
}

void GoldenCudgel_grow_to(GoldenCudgel self, vptr to)
{
    if (to > (vptr)self->tail)
    {
        ref_ptr grow = (ref_ptr)to - (ref_ptr)self->tail;
        euint num_eles = grow / self->ele_size;
        if (grow % self->ele_size)
        {
            num_eles++;
        }
        GoldenCudgel_grow_up_not_fill(self, num_eles);
    }
}
