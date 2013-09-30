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
