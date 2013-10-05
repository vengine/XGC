/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef LIST_BASE_H
#define LIST_BASE_H

typedef struct _list
{
	MALLOC alloc_proc;
	MFREE free_proc;
	struct list_node* head;
	struct list_node* tail;
	euint count;
	etype value_type;
#ifdef ALLOW_CONCURRENT
	ELock elock;
#endif
}list;

_INLINE_ void ListFunc(node_Init)(struct list_node* _node, var _data)
{
	_node->iter_prev = NULL;
	_node->iter_next = NULL;
	_node->value = _data;
}
List ListFunc(Init)(List _lst, etype _value_type, MALLOC _alc, MFREE _fre)
{
	list* lst = (list*)_lst;
	lst->head = NULL;
	lst->tail = NULL;
	lst->count = 0;
	lst->value_type = _value_type;
	lst->alloc_proc = _alc;
	lst->free_proc = _fre;
#ifdef ALLOW_CONCURRENT
	ELock_Init(&lst->elock);
#endif
	return _lst;
}
List ListFunc(new)(etype _value_type, MALLOC _alc, MFREE _fre)
{
	list* ret = (list*)_alc(sizeof(list));
	return List_Init(ret, _value_type, _alc, _fre);
}
void ListFunc(Dest)(List _lst)
{
	Iterator iter = List_begin(_lst);
	while (iter)
	{
		iter = List_remove(_lst, iter);
	}
}

Iterator ListFunc(push_back)(List _lst, var _data)
{
	struct list_node* node = (struct list_node*)_lst->alloc_proc(sizeof(struct list_node));
	Iterator ret = NULL;
	ListFunc(node_Init)(node, _data);
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_lst->elock);
#endif
	if (_lst->tail)
	{
		_lst->tail->iter_next = node;
		node->iter_prev = _lst->tail;
		node->iter_next = NULL;
		_lst->tail = node;
	}
	else
	{
		_lst->head = _lst->tail = node;
		_lst->tail->iter_prev = NULL;
		_lst->tail->iter_next = NULL;
	}
	_lst->count++;
	ret = _lst->tail;
#ifdef ALLOW_CONCURRENT
	ELock_unlock(&_lst->elock);
#endif
	return ret;
}

Iterator ListFunc(push_front)(List _lst, var _data)
{
	struct list_node* node = (struct list_node*)_lst->alloc_proc(sizeof(struct list_node));
	Iterator ret = NULL;
	ListFunc(node_Init)(node, _data);
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_lst->elock);
#endif
	if (_lst->head)
	{
		_lst->head->iter_prev = node;
		node->iter_next = _lst->head;
		node->iter_prev = NULL;
		_lst->head = node;
	}
	else
	{
		_lst->tail = _lst->head = node;
		_lst->head->iter_prev = NULL;
		_lst->head->iter_next = NULL;
	}
	_lst->count++;
	ret = _lst->head;
#ifdef ALLOW_CONCURRENT
	ELock_unlock(&_lst->elock);
#endif
	return ret;
}

Iterator ListFunc(insert_after)(List _lst, Iterator _i, var _data)
{
	struct list_node* node = (struct list_node*)_i;
	struct list_node* n = (struct list_node*)_lst->alloc_proc(sizeof(struct list_node));
	ListFunc(node_Init)(n, _data);
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_lst->elock);
#endif
	if (node->iter_next)
	{
		n->iter_next = node->iter_next;
		node->iter_next->iter_prev = n;
		n->iter_prev = node;
		node->iter_next = n;
		_lst->count++;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
		return n;
	}
	else
	{
		Iterator ret = NULL;
		node->iter_next = n;
		_lst->tail = node->iter_next;
		_lst->tail->iter_prev = node;
		_lst->tail->iter_next = NULL;
		_lst->count++;
		ret = _lst->tail;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
		return ret;
	}
}

Iterator ListFunc(insert_before)(List _lst, Iterator _i, var _data)
{
	struct list_node* node = (struct list_node*)_i;
	struct list_node* n = (struct list_node*)_lst->alloc_proc(sizeof(struct list_node));
	ListFunc(node_Init)(n, _data);
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_lst->elock);
#endif
	if (node->iter_prev)
	{
		n->iter_prev = node->iter_prev;
		node->iter_prev->iter_next = n;
		n->iter_next = node;
		node->iter_prev = n;
		_lst->count++;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
		return n;
	}
	else
	{
		Iterator ret = NULL;
		node->iter_prev = n;
		_lst->head = node->iter_prev;
		_lst->head->iter_prev = NULL;
		_lst->head->iter_next = node;
		_lst->count++;
		ret = _lst->head;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
		return ret;
	}
}

Iterator ListFunc(remove)(List _lst, Iterator _i)
{
	if (!_lst->count)
	{
		return NULL;
	}
	else
	{
		struct list_node* node = (struct list_node*)_i;
#ifdef ALLOW_CONCURRENT
		ELock_lock(&_lst->elock);
#endif
		_lst->count--;
		if (node != _lst->head && node != _lst->tail)
		{
			struct list_node* ret = node->iter_next;
			node->iter_prev->iter_next = node->iter_next;
			node->iter_next->iter_prev = node->iter_prev;
#ifdef ALLOW_CONCURRENT
			ELock_unlock(&_lst->elock);
#endif
			_lst->free_proc((vptr)node);
			return ret;
		}
		else if (node != _lst->head && node == _lst->tail)
		{
			node->iter_prev->iter_next = NULL;
			_lst->tail = node->iter_prev;
#ifdef ALLOW_CONCURRENT
			ELock_unlock(&_lst->elock);
#endif
			_lst->free_proc(node);
			return NULL;
		}
		else if (node == _lst->head && node != _lst->tail)
		{
			struct list_node* ret = node->iter_next;
			node->iter_next->iter_prev = NULL;
			_lst->head = node->iter_next;
#ifdef ALLOW_CONCURRENT
			ELock_unlock(&_lst->elock);
#endif
			_lst->free_proc(node);
			return ret;
		}
		else
		{
			struct list_node* tmp = _lst->head;
			_lst->head = _lst->tail = NULL;
#ifdef ALLOW_CONCURRENT
			ELock_unlock(&_lst->elock);
#endif
			_lst->free_proc(tmp);
			return NULL;
		}
	}
}

Iterator ListFunc(next)(Iterator _i)
{
	return ((struct list_node*)_i)->iter_next;
}

Iterator ListFunc(prev)(Iterator _i)
{
	return ((struct list_node*)_i)->iter_prev;
}

Iterator ListFunc(begin)(List _lst)
{
	return _lst->head;
}

Iterator ListFunc(end)(List _lst)
{
	return _lst->tail;
}

List ListFunc(append)(List _head, List _tail)
{
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_head->elock);
	ELock_lock(&_tail->elock);
#endif
	if (!_head || !_tail)
	{
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_head->elock);
		ELock_unlock(&_tail->elock);
#endif
		return NULL;
	}
	if (_head->value_type != _tail->value_type)
	{
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_head->elock);
		ELock_unlock(&_tail->elock);
#endif
		return NULL;
	}
	if (_head->count && _tail->count)
	{
		_head->count += _tail->count;
		_head->tail = _tail->tail;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_head->elock);
		ELock_unlock(&_tail->elock);
#endif
		return _head;
	}
	else if (!_head->count && _tail->count)
	{
		_head->count = _tail->count;
		_head->head = _tail->head;
		_head->tail = _tail->tail;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_head->elock);
		ELock_unlock(&_tail->elock);
#endif
		return _head;
	}
	else if (_head->count && !_tail->count)
	{
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_head->elock);
		ELock_unlock(&_tail->elock);
#endif
		return _head;
	}
	else if (!_head->count && !_tail->count)
	{
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_head->elock);
		ELock_unlock(&_tail->elock);
#endif
		return _head;
	}
#ifdef ALLOW_CONCURRENT
	ELock_unlock(&_head->elock);
	ELock_unlock(&_tail->elock);
#endif
	return NULL;
}

List ListFunc(break)(List _lst, Iterator _i)
{
	struct list_node* prev = NULL;
#ifdef ALLOW_CONCURRENT
	ELock_lock(&_lst->elock);
#endif
	if (!_lst || !_i)
	{
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
		return NULL;
	}

	prev = ((struct list_node*)_i)->iter_prev;
	if (prev)
	{
		List ret = NULL;
		euint32 count = 0;
		struct list_node* iter = (struct list_node*)_i;
		while (iter)
		{
			count++;
			iter = iter->iter_next;
		}
		if (!count)
		{
#ifdef ALLOW_CONCURRENT
			ELock_unlock(&_lst->elock);
#endif
			return NULL;
		}
		ret = (List)_lst->alloc_proc(sizeof(list));
		ret->head = (struct list_node*)_i;
		ret->tail = _lst->tail;
		ret->count = count;
		ret->value_type = _lst->value_type;

		_lst->count -= count;
		_lst->tail = prev;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
		return ret;
	}
	else
	{
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
		return NULL;
	}
}

euint ListFunc(count)(List _lst)
{
	return _lst->count;
}

var ListFunc(get_value)(Iterator _i)
{
	return ((struct list_node*)_i)->value;
}

void ListFunc(throw_front)(List _lst, Iterator _i)
{
	if (!_lst->count)
		return;
	{
		struct list_node* node = (struct list_node*)_i;
#ifdef ALLOW_CONCURRENT
		ELock_lock(&_lst->elock);
#endif
		if (node != _lst->head && node != _lst->tail)
		{
			node->iter_prev->iter_next = node->iter_next;
			node->iter_next->iter_prev = node->iter_prev;
		}
		else if (node != _lst->head && node == _lst->tail)
		{
			node->iter_prev->iter_next = NULL;
			_lst->tail = node->iter_prev;
		}
		else
		{
#ifdef ALLOW_CONCURRENT
			ELock_unlock(&_lst->elock);
#endif
			return;
		}

		_lst->head->iter_prev = node;
		node->iter_next = _lst->head;
		node->iter_prev = NULL;
		_lst->head = node;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
	}
}

void ListFunc(throw_back)(List _lst, Iterator _i)
{
	if (!_lst->count)
		return;
	{
		struct list_node* node = (struct list_node*)_i;
#ifdef ALLOW_CONCURRENT
		ELock_lock(&_lst->elock);
#endif
		if (node != _lst->head && node != _lst->tail)
		{
			node->iter_prev->iter_next = node->iter_next;
			node->iter_next->iter_prev = node->iter_prev;
		}
		else if (node == _lst->head && node != _lst->tail)
		{
			node->iter_next->iter_prev = NULL;
			_lst->head = node->iter_next;
		}
		else
		{
#ifdef ALLOW_CONCURRENT
			ELock_unlock(&_lst->elock);
#endif
			return;
		}

		_lst->tail->iter_next = node;
		node->iter_prev = _lst->tail;
		node->iter_next = NULL;
		_lst->tail = node;
#ifdef ALLOW_CONCURRENT
		ELock_unlock(&_lst->elock);
#endif
	}
}
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