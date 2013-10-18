/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef RWBUFFER_H
#define RWBUFFER_H
#include "common.h"
#include "etypes.h"
#include "emem.hpp"
API_EXPORT RWBuffer RWBuffer_new(euint buffer_size);
API_EXPORT void RWBuffer_delete(RWBuffer _self);
API_EXPORT bool RWBuffer_Read(RWBuffer _self, euint* result, euint* read_size);
API_EXPORT bool RWBuffer_Write(RWBuffer _self, const euint* from, const euint write_size);
API_EXPORT bool RWBuffer_IsEmpty(RWBuffer _self);

class SafedBuffer : public MemObject
{
public:
    char* m_transferBuffer;
    RWBuffer m_buffer;
    euint64 m_nonblockingCount;
	euint64 m_blockingCount;
public:
    SafedBuffer(euint bufferSize);
    ~SafedBuffer();
    template <typename T>
    void Write(const T& from) {
		euint tmp[ (sizeof(T) % sizeof(euint)) ? 
			        sizeof(T) / sizeof(euint) * sizeof(euint) + sizeof(euint) :
		            sizeof(T) / sizeof(euint) ];
		memset(tmp, 0, sizeof(tmp));
		memcpy(tmp, (vptr)&from, sizeof(T));
		while (!RWBuffer_Write(m_buffer, tmp, sizeof(T))) {
            m_blockingCount++;
        }
        m_nonblockingCount++;
    }
	void Write(const void* buf, euint size);
    char* Read(euint* readSize);
	inline bool IsEmpty() {
		return RWBuffer_IsEmpty(m_buffer);
	}
    inline float GetBlockrate()
    {
        float ret = (float)((double)m_blockingCount / (double)(m_blockingCount + m_nonblockingCount));
        m_blockingCount = 0;
        m_nonblockingCount = 0;
        return ret;
    }
};
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