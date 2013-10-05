/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef TREE_H
#define TREE_H
#include "common.h"
#include "etypes.h"
#include "comparison_algorithm.h"
typedef void (*DataDestProc)(var);

API_EXPORT Tree Tree_new(etype _key_type, etype _value_type, MALLOC _alc, MFREE _fre);
API_EXPORT Tree Tree_Init(struct _tree* _tree, etype _key_type, etype _value_type, MALLOC _alc, MFREE _fre);
API_EXPORT void Tree_Dest(Tree _tree);
API_EXPORT void Tree_remove(Tree _tree, var _key);
API_EXPORT Iterator Tree_insert(Tree _tree, var _key, var _data);
API_EXPORT Iterator Tree_replace(Tree _tree, var _key, var _data);
API_EXPORT Iterator Tree_force_find(Tree _tree, var _key);
API_EXPORT Iterator Tree_find(Tree _tree, var _key, var* _to);
API_EXPORT var Tree_get_key(Iterator _i);
API_EXPORT var Tree_get_value(Iterator _i);
API_EXPORT void Tree_set_value(Iterator _i, var _data);
API_EXPORT Iterator Tree_next(Iterator _i);
API_EXPORT Iterator Tree_prev(Iterator _i);
API_EXPORT Iterator Tree_begin(Tree _tree);
API_EXPORT Iterator Tree_end(Tree _tree);
API_EXPORT euint Tree_count(Tree _tree);

API_EXPORT void Tree_set_key_compare_proc(Tree _tree, KEY_COMPARE _proc);
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