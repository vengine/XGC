/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef MAP_H
#define MAP_H
#include "common.h"
#include "etypes.h"
#include "tree.h"
template <typename KEY, typename DATA>
class TTMap
{
public:
	typedef void (*TTDest)(Tree);
	typedef int (*KeyCompareProc)(KEY*, KEY*);

	class TTIterator
	{
		friend class TTMap<KEY, DATA>;
	private:
		Iterator m_iter;
	public:
		TTIterator(Iterator iter)
			: m_iter(iter)
		{}
		TTIterator(const TTIterator& iter)
			: m_iter(iter.m_iter)
		{}
	public:
		DATA* GetData()
		{
			var ret = Tree_get_value(m_iter);
			return (DATA*)ret.vptr_var;
		}
		KEY* GetKey()
		{
			var key = Tree_get_key(m_iter);
			return (KEY*)key.vptr_var;
		}
		TTIterator Next()
		{
			Iterator iter = Tree_next(m_iter);
			return TTIterator(iter);
		}
		TTIterator Prev()
		{
			Iterator iter = Tree_prev(m_iter);
			return TTIterator(iter);
		}
		bool IsValid()
		{
			return m_iter != NULL;
		}
	};
private:
	TTDest      m_destProc;
	Tree        m_tree;
public:
	TTMap(TTDest destProc, KeyCompareProc compareProc)
		: m_destProc(destProc)
	{
		m_tree = Tree_new(Vptr, Vptr, (MALLOC)Ealloc, (MFREE)Efree);
		Tree_set_key_compare_proc(m_tree, (KEY_COMPARE)compareProc);
	}
	~TTMap()
	{
		m_destProc(m_tree);
	}
	TTIterator Find(KEY* _key)
	{
		var key, data;
		key.vptr_var = (vptr)_key;
		Iterator iter = Tree_find(m_tree, key, &data);
		return TTIterator(iter);
	}
	TTIterator Begin()
	{
		return TTIterator(Tree_begin(m_tree));
	}
	TTIterator Insert(KEY* _key, DATA* _data)
	{
		var key, data;
		key.vptr_var = (vptr)_key;
		data.vptr_var = (vptr)_data;
		return TTIterator(Tree_insert(m_tree, key, data));
	}
	void Remove(KEY* _key)
	{
		var key;
		key.vptr_var = (vptr)_key;
		Tree_remove(m_tree, key);
	}
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