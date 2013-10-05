/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */
#ifndef CONTAINER_H
#define CONTAINER_H
#include "common.h"
#include "etypes.h"
#ifndef __APPLE__
#include <pthread.h>
#endif
struct list_node
{
    struct list_node* iter_prev;
    struct list_node* iter_next;
    var value;
};
typedef struct _stack
{
    var* stack;
    euint stack_length;
    euint stack_top_ptr;
    etype value_type;
}stack;

typedef struct _fixed_stack
{
    void* stack;
    euint stack_length;
    euint stack_top_ptr;
    euint value_size;
} fixed_stack;
#include "tree.h"

typedef struct _tree
{
    MALLOC alloc_proc;
    MFREE free_proc;
    KEY_COMPARE key_compare_proc;
    struct tree_node* root;
    struct tree_node* head;
    struct tree_node* tail;
    euint count;
    etype key_type;
    etype value_type;
}tree;

typedef enum
{
    Black = 0,
    Red = 1,
}node_color;
struct tree_node
{
    struct tree_node *left_node;
    struct tree_node *right_node;
    struct tree_node *parent_node;
    struct tree_node *iter_prev;
    struct tree_node *iter_next;
    var key;
    var value;
    node_color color;
    struct _tree* ower;
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
