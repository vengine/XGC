#ifndef LIST_H
#define LIST_H
#include "common.h"
#include "etypes.h"
struct _list;
typedef struct _list* List;
API_EXPORT List List_new(etype _value_type, MALLOC _alc, MFREE _fre);
API_EXPORT List List_Init(List _lst, etype _value_type, MALLOC _alc, MFREE _fre);
API_EXPORT void List_Dest(List _lst);
API_EXPORT Iterator List_push_back(List _lst, var _data);
API_EXPORT Iterator List_push_front(List _lst, var _data);
API_EXPORT Iterator List_insert_after(List _lst, Iterator _i, var _data);
API_EXPORT Iterator List_insert_before(List _lst, Iterator _i, var _data);
API_EXPORT Iterator List_remove(List _lst, Iterator _i);
API_EXPORT Iterator List_next(Iterator _i);
API_EXPORT Iterator List_prev(Iterator _i);
API_EXPORT Iterator List_begin(List _lst);
API_EXPORT Iterator List_end(List _lst);
API_EXPORT List List_append(List _head, List _tail);
API_EXPORT List List_break(List _lst, Iterator _i);
API_EXPORT euint List_count(List _lst);
API_EXPORT var List_get_value(Iterator _i);
API_EXPORT void List_throw_front(List _lst, Iterator _i);
API_EXPORT void List_throw_back(List _lst, Iterator _i);
#endif
