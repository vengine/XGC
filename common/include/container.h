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
