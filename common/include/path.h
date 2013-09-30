#ifndef PATH_H
#define PATH_H
#include "common.h"
#include "etypes.h"
#include "estring.h"
#include "tree.h"
#include "array.h"
#include "emem.h"

typedef EString* EStringArray;
_INLINE_ EStringArray EStringArray_new()
{
    return array_new(EString, 5, NULL);
}
_INLINE_ void EStringArray_delete(EStringArray _self)
{
    euint n = array_n(_self);
    for (euint i = 0; i < n; i++)
    {
        EString_delete(_self[i]);
    }
    array_delete(_self);
}
void GetAllFileNamesInDir(const char* dir, const char* filter_rule, EStringArray* result);
#endif
