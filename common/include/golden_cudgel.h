#ifndef GOLDEN_CUDGEL_H
#define GOLDEN_CUDGEL_H
#include "common.h"
#include "etypes.h"
typedef struct _golden_cudgel* GoldenCudgel;

API_EXPORT GoldenCudgel GoldenCudgel_new(euint ele_size);
API_EXPORT void GoldenCudgel_delete(GoldenCudgel self);
API_EXPORT euint GoldenCudgel_get_element_size(GoldenCudgel self);
API_EXPORT void GoldenCudgel_grow_up(GoldenCudgel self, vptr eles, euint ele_count);
API_EXPORT void GoldenCudgel_grow_up_not_fill(GoldenCudgel self, euint ele_count);
API_EXPORT void GoldenCudgel_cut_down(GoldenCudgel self, euint ele_count);
API_EXPORT vptr GoldenCudgel_get_element_pointer(GoldenCudgel self, euint i);
API_EXPORT void GoldenCudgel_grow_to(GoldenCudgel self, vptr to);
#endif
