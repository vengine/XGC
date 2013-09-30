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
