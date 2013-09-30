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
