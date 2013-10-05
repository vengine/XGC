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
#include "rwbuffer.h"
#include "emem.h"
/// |0x0000                 |0x0004                 |0x0008                  |0x000c
/// |0x00 |0x01 |0x02 |0x03 |0x04 |0x05 |0x06 |0x07 | 0x08 |0x09 |0x0a |0x0b |0x0c |0x0d |0x0e |0x0f
/// |top barrier                                    |bottom barrier          |
/// |real top barrier                                     real bottom barrier|

#define GetRealSize(type, size) size + ( sizeof(type) - (size % sizeof(type)) )

struct rw_buffer
{
    const euint* top_barrier;
    const euint* bottom_barrier;
    volatile euint* top_pointer;
    volatile euint* bottom_pointer;
};

RWBuffer RWBuffer_new(euint buffer_size)
{
    RWBuffer ret;
    euint real_size = 0;
    euint number_uints = 0;
    ret = (rw_buffer*)SMalloc(sizeof(rw_buffer));

    real_size = GetRealSize(euint, buffer_size);
    number_uints = real_size / sizeof(euint);
    ret->top_barrier = (euint*)SMalloc(real_size);
    ret->bottom_barrier = ret->top_barrier + number_uints;
    ret->top_pointer = (volatile euint*)ret->top_barrier;
    ret->bottom_pointer = (volatile euint*)ret->top_barrier;
    return ret;
}

void RWBuffer_delete(RWBuffer _self)
{
    if (_self->top_barrier)
    {
        Mfree((void*)_self->top_barrier);
        _self->top_barrier = NULL;
        _self->bottom_barrier = NULL;
    }
	if (_self)
	    Mfree(_self);
}

bool RWBuffer_Read(RWBuffer _self, euint* result, euint* read_size)
{
    register euint* registered_top_pointer = (euint*)_self->top_pointer;
    euint* buffer_chunk_pointer = NULL;
    euint number_uints = 0;
    euint* next = NULL;
    euint i = 0;

    if (registered_top_pointer == _self->bottom_pointer)
        return false;

    buffer_chunk_pointer = registered_top_pointer;
    *read_size = *buffer_chunk_pointer;
    buffer_chunk_pointer++;

    if (buffer_chunk_pointer > _self->bottom_barrier)
        buffer_chunk_pointer = (euint*)_self->top_barrier;

    number_uints = *read_size / sizeof(euint);
	if (*read_size % sizeof(euint))
		number_uints++;

    next = (euint*)*buffer_chunk_pointer;
    buffer_chunk_pointer++;

    if (buffer_chunk_pointer > _self->bottom_barrier)
        buffer_chunk_pointer = (euint*)_self->top_barrier;

    for (i = 0; i < number_uints; i++)
    {
        if (buffer_chunk_pointer > _self->bottom_barrier)
            buffer_chunk_pointer = (euint*)_self->top_barrier;

        result[i] = *buffer_chunk_pointer;
        buffer_chunk_pointer++;
    }

    _self->top_pointer = next;
    return true;
}

bool RWBuffer_Write(RWBuffer _self, const euint* from, const euint write_size)
{
    register euint* registered_top_pointer = (euint*)_self->top_pointer;
    register euint* registered_bottom_pointer = (euint*)_self->bottom_pointer;
    euint real_write_size = GetRealSize(euint, write_size);
    euint number_uints = real_write_size / sizeof(euint);
    euint* buffer_chunk_pointer = registered_bottom_pointer + 2;
    euint i = 0;

	if (!write_size)
		return false;

    for (i = 0; i < number_uints; i++)
    {
        if (buffer_chunk_pointer > _self->bottom_barrier)
            buffer_chunk_pointer = (euint*)_self->top_barrier;

        if (buffer_chunk_pointer == registered_top_pointer)
            return false;

        *buffer_chunk_pointer = from[i];
        buffer_chunk_pointer++;
    }

    if (buffer_chunk_pointer > _self->bottom_barrier)
        buffer_chunk_pointer = (euint*)_self->top_barrier;

    *registered_bottom_pointer = write_size;
    registered_bottom_pointer++;
    if (registered_bottom_pointer > _self->bottom_barrier)
        registered_bottom_pointer = (euint*)_self->top_barrier;
    *registered_bottom_pointer = (euint)buffer_chunk_pointer;

    _self->bottom_pointer = buffer_chunk_pointer;
    return true;
}

bool RWBuffer_IsEmpty(RWBuffer _self)
{
	register euint* registered_top_pointer = (euint*)_self->top_pointer;
	return registered_top_pointer == _self->bottom_pointer;
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