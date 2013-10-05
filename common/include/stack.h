/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef STACK_H
#define STACK_H
#include "common.h"
#include "etypes.h"
#define STACK_INITIAL_LENGTH 4
struct _stack;
typedef struct _stack* Stack;
API_EXPORT Stack Stack_new(etype _value_type);
API_EXPORT Stack Stack_Init(Stack _stk, etype _value_type);
API_EXPORT void Stack_Dest(Stack _stk);
API_EXPORT void Stack_push(Stack _stk, var _data);
API_EXPORT bool Stack_pop(Stack _stk, var* _to);
API_EXPORT bool Stack_get_top(Stack _stk, var* _to);
API_EXPORT void Stack_clear(Stack _stk);
API_EXPORT void Stack_resize(Stack _stk, euint _size);
API_EXPORT euint Stack_count(Stack _stk);
API_EXPORT var* Stack_find(Stack _stk, euint _i);

struct _fixed_stack;
typedef struct _fixed_stack* FixedStack;
API_EXPORT FixedStack FixedStack_new(euint _value_size);
API_EXPORT void FixedStack_Dest(FixedStack _stk);
API_EXPORT void FixedStack_push(FixedStack _stk, void* _data);
API_EXPORT bool FixedStack_pop(FixedStack _stk, void* _to);
API_EXPORT bool FixedStack_get_top(FixedStack _stk, void* _to);
API_EXPORT void* FixedStack_get_top_ptr(FixedStack _stk);
API_EXPORT bool FixedStack_walk_begin(FixedStack _stk, void* _to, euint* _top_ptr);
API_EXPORT bool FixedStack_walk(FixedStack _stk, void* _to, euint* _top_ptr);
API_EXPORT void FixedStack_clear(FixedStack _stk);
API_EXPORT void FixedStack_resize(FixedStack _stk, euint _size);
API_EXPORT euint FixedStack_count(FixedStack _stk);
API_EXPORT void* FixedStack_find(FixedStack _stk, euint _i);

API_EXPORT void FixedStack_test();
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
