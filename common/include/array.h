/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef ARRAY_H
#define ARRAY_H
#include "common.h"
#include "etypes.h"
#include "emem.h"

#define ARRAY

#ifdef __GNUC__
#define apush(a, e) a = array_push(a, e)

#define fexce ({float _e_; *((unsigned int*)&_e_) = 0xffffffff; _e_;})

#define Array_Initialize_Size 5
#define d_malloc(n, t) SMalloc( (n + 1) * sizeof(t) + sizeof(unsigned int) * 2 )
#define d_free(a) Mfree( a )

#define array_max(a) ((unsigned int*)&(a)[-1])[-2]
#define array_n(a) ((unsigned int*)&(a)[-1])[-1]
#define get_exce(a) (a[-1])
#define array_new(t, n, exce) ({t* _a_ = (t*)d_malloc(n, t); \
                              ((unsigned int*)_a_)[0] = n; \
                              ((unsigned int*)_a_)[1] = 0; \
                              ((t*)&((unsigned int*)_a_)[2])[0] = exce; \
                              &((t*)&((unsigned int*)_a_)[2])[1];} )
#define array_delete(a) d_free( &((unsigned int*)&(a)[-1])[-2] )

#define array_safe_get(a, i) ( (i < array_n(a) && i >= 0) ? (a)[i] : get_exce(a) )
#define array_safe_get_ptr(a, i) ( (i < array_n(a) && i >= 0) ? &(a)[i] : NULL )
#define array_safe_set(a, i, e) ( (i < array_n(a) && i >= 0) ? (a)[i] = e : get_exce(a) )
#define array_push(a, e) ({ \
                             (a)[array_n(a)] = e; \
                             array_n(a)++; \
                             (array_n(a) < array_max(a)) ? (a) \
                             : \
                             ({ \
                                 typeof((a)) _b_; \
                                 _b_ = (typeof((a)))d_malloc( array_max(a) * 2, typeof(*(a)) ); \
                                 ((unsigned int*)_b_)[0] = array_max(a) * 2; \
                                 ((unsigned int*)_b_)[1] = array_n(a); \
                                 ((typeof((a)))&((unsigned int*)_b_)[2])[0] = (a)[-1];\
                                 _b_ = &((typeof((a)))&((unsigned int*)_b_)[2])[1]; \
                                 memcpy(_b_, (a), sizeof( typeof(*(a)) ) * array_n((a)) ); \
                                 d_free( &((unsigned int*)&(a)[-1])[-2] ); \
                                 _b_; \
                              });\
                          })

#define array_pop(a) (array_n(a) > 0) ? ({ typeof(*(a)) _e_ = (a)[array_n(a) - 1]; array_n(a)--; _e_;}) : (a)[-1]
#define array_grow(a, n) ({ \
                              typeof((a)) _b_; \
                              _b_ = d_malloc( array_max(a) + n, typeof(*(a)) ); \
                              ((unsigned int*)_b_)[0] = array_max(a) + n; \
                              ((unsigned int*)_b_)[1] = array_n(a); \
                              ((typeof((a)))&((unsigned int*)_b_)[2])[0] = (a)[-1];\
                              _b_ = &((typeof((a)))&((unsigned int*)_b_)[2])[1]; \
                              memcpy(_b_, a, sizeof( typeof(*(a)) ) * array_n((a)) ); \
                              d_free( &((unsigned int*)&(a)[-1])[-2] ); \
                              _b_; \
                          })

#define array_resize(a, n) (array_n(a) < n) ? ({euint _count_ = (n) - array_n(a); for (euint _i_ = 0; _i_ < _count_; _i_++) a = array_push(a, get_exce(a)); a;}) : ({array_n(a) = n; a;})
#elif defined _MSC_VER
#pragma warning(push)
#pragma warning(disable:4200)
template<typename T>
struct Array
{
	euint totel_size;
	euint curt_size;
	T exce_value;
	T array[0];
};
template<typename T>
T* ArrayNew(T exce, euint size)
{
	Array<T>* ret = (Array<T>*)SMalloc(sizeof(Array<T>) + sizeof(T) * size);
	ret->totel_size = size;
	ret->curt_size = 0;
	ret->exce_value = exce;
	memset(ret->array, 0, sizeof(T) * size);
	return ret->array;
}
template<typename T>
T* ArrayPush(T* a, T v)
{
    Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
	if (arr->curt_size + 1 >= arr->totel_size)
	{
		Array<T>* tmp = (Array<T>*)SMalloc(sizeof(Array<T>) + sizeof(T) * arr->totel_size * 2);
		memcpy(tmp->array, arr->array, sizeof(T) * arr->curt_size);
		tmp->curt_size = arr->curt_size;
		tmp->totel_size = arr->totel_size * 2;
		tmp->exce_value = arr->exce_value;
		Mfree(arr);
		arr = tmp;
	}
	arr->array[arr->curt_size] = v;
	arr->curt_size++;
	return arr->array;
}
template<typename T>
void ArrayDelete(T* a)
{
    Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
	Mfree(arr);
}
template<typename T>
T ArraySafeGet(T* a, euint i)
{
	Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
    if (i < arr->curt_size)
		return arr->array[i];
	else
		return arr->exce_value;
}
template<typename T>
T* ArraySafeGetPtr(T* a, euint i)
{
	Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
	if (i < arr->curt_size)
		return arr->array[i];
	else
		return NULL;
}
template<typename T>
void ArraySafeSet(T* a, euint i, T v)
{
	Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
	if (i < arr->curt_size)
		arr->array[i] = v;
}
template <typename T>
T ArrayPop(T* a)
{
    Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
	if (arr->curt_size)
	{
		arr->curt_size--;
		return arr->array[arr->curt_size];
	}
	else
	{
		return arr->exce_value;
	}
}
template <typename T>
T* ArrayGrow(T* a, euint n)
{
    if (n)
	{
        Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
		Array<T>* tmp = (Array<T>*)SMalloc(sizeof(Array<T>) + sizeof(T) * (arr->totel_size + n));
		memcpy(tmp->array, arr->array, sizeof(T) * arr->curt_size);
		tmp->exce_value = arr->exce_value;
		tmp->totel_size = arr->totel_size + n;
		tmp->curt_size = arr->curt_size;
		Mfree(arr);
		return tmp->array;
	}
	else
		return a;
}
template <typename T>
T* ArrayResize(T* a, euint n)
{
	Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
	if (n < arr->totel_size) {
		arr->curt_size = n;
		return arr->array;
	}
	else {
        T* ret = ArrayGrow(a, arr->totel_size * 2);
        ArrayNum(ret) = n;
		return ret;
	}
}
template <typename T>
euint ArrayMax(T* a)
{
    Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
	return arr->totel_size;
}
template <typename T>
euint& ArrayNum(T* a)
{
	Array<T>* arr = (Array<T>*)((char*)a - sizeof(Array<T>));
	return arr->curt_size;
}
#define array_new(t, n, exce) ArrayNew<t>(exce, n)
#define array_delete(a) ArrayDelete(a)
#define array_max(a) ArrayMax(a)
#define array_n(a) ArrayNum(a)
#define array_push(a, e) ArrayPush(a, e);
#define apush(a, e) a = array_push(a, e)
#define array_safe_get(a, i) ArraySafeGet(a, i)
#define array_safe_get_ptr(a, i) ArraySafeGetPtr(a, i)
#define array_pop(a) ArrayPop(a)
#define array_resize(a, n) ArrayResize(a, n)
inline float _fexce()
{
    float ret;
	*((euint32*)&ret) = 0xffffffff;
	return ret;
}
#define fexce _fexce()

#pragma warning(pop)
#endif
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
