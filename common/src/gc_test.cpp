#include "pch.h"
#include "resource_node.h"

class int_value
{
private:
	ResourceNode* m_node;
public:
    int_value(int v)
	{
        vptr mem = SMalloc(sizeof(int));
		memcpy(mem, &v, sizeof(v));
		m_node = ENEW ResourceNode(mem);
	}
	int_value()
	{
		m_node = ENEW ResourceNode();
	}
    ~int_value()
	{
		///m_node->MarkDeleted();
		m_node->TrackBack();
	}
	int Get() 
	{
		return *((int*)m_node->Get());
	}
	void Set(int i)
	{
		*((int*)m_node->Get()) = i;
	}
	void Attach(int_value& v)
	{
		m_node->Attach(v.m_node);
	}
};
void cale(int_value& a, int_value& b, int_value& ret)
{
    int_value tmp0(a.Get());
	int_value tmp1(b.Get());
	ret.Set(tmp0.Get() + tmp1.Get());
}
void tt1(int_value& root)
{
	int_value tmp0(1);
	int_value tmp1(2);
	tmp0.Attach(tmp1);
	tmp1.Attach(tmp0);
	root.Attach(tmp0);
}
void tt()
{
	int_value root;
	int_value tmp0(1);
	int_value tmp1(2);
	tmp0.Attach(tmp1);
	tmp1.Attach(tmp0);
	root.Attach(tmp0);
	root.Attach(tmp1);
	tt1(tmp1);
	tt1(tmp0);
}
void gc_test()
{
	/**
    int_value i0(1);
    int_value i1(2);
	int_value r(0);
	cale(i0, i1, r);
	ResourceNode::Update();
	printf("%d\n", r.Get());
	ResourceNode::Update();
	**/
	tt();
    ResourceNode::Update();
	ResourceNode::Update();
	ResourceNode::Update();
	ResourceNode::Update();
}