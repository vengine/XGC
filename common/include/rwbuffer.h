#ifndef RWBUFFER_H
#define RWBUFFER_H
#include "common.h"
#include "etypes.h"
API_EXPORT RWBuffer RWBuffer_new(euint buffer_size);
API_EXPORT void RWBuffer_delete(RWBuffer _self);
API_EXPORT bool RWBuffer_Read(RWBuffer _self, euint* result, euint* read_size);
API_EXPORT bool RWBuffer_Write(RWBuffer _self, const euint* from, const euint write_size);
API_EXPORT bool RWBuffer_IsEmpty(RWBuffer _self);
#endif
