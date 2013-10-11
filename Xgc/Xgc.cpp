// Xgc.cpp : Defines the entry point for the console application.
//

#include "xhn_garbage_collector.hpp"
#include "rwbuffer.h"
class ListNode;
typedef xhn::garbage_collector::mem_handle<ListNode> ListNodeHandle;
class ListNode
{
public:
    ListNodeHandle m_prev;
	ListNodeHandle m_next;
	int m_value;
	ListNode()
	{
    }
    const ListNodeHandle& Next() {
        return m_next;
    }
	void Erase() {
		if (m_next) { m_next->m_prev = m_prev; }
		if (m_prev) { m_prev->m_next = m_next; }
	}
};

class List
{
public:
	ListNodeHandle m_head;
	ListNodeHandle m_tail;
public:
	void PushBack(int value) {
        ListNodeHandle node;
		node = GC_ALLOC(ListNode);
		node->m_value = value;
		if (m_tail) {
            m_tail->m_next = node;
            node->m_prev = m_tail;
			m_tail = node;
		}
		else {
			m_head = m_tail = node;
		}
	}
    const ListNodeHandle& Begin() {
        return m_head;
    }
	void Print() {
		ListNodeHandle node;
		node = m_head;
		while(node) {
			printf("##%d\n", node->m_value);
			node = node->m_next;
		}
	}
};

typedef xhn::garbage_collector::mem_handle<List> ListHandle;

typedef xhn::garbage_collector::mem_handle<int> IntHandle;
typedef xhn::garbage_collector::mem_handle<float> FloatHandle;
struct TestObject
{
	IntHandle handle0;
	IntHandle handle1;
	FloatHandle handle2;
	xhn::garbage_collector::mem_handle<TestObject> hh;
};
typedef xhn::garbage_collector::mem_handle<TestObject> TestObjectHandle;
int main(void)
{
    IntHandle intHandle;
    intHandle = GC_ALLOC(int);
    *intHandle = 0;
	while (1)
	{
		TestObjectHandle handle;
		handle = GC_ALLOC(TestObject);
		handle->handle0 = GC_ALLOC(int);
		handle->handle1 = GC_ALLOC(int);
		
		*handle->handle0 = 100;
		*handle->handle1 = 200;
		handle->handle2 = GC_ALLOC(float);

		*handle->handle2 = (float)(*handle->handle0 + *handle->handle1);
		handle->hh = handle;
		ListHandle list;
		ListNodeHandle iter;
        GC_ALLOC(List);
		list = GC_ALLOC(List);
		
		for (int i = 0; i < 4; i++) {
			list->PushBack(i);
		}
        (*intHandle)++;
		
		if ((*intHandle % 10000) == 0) {
			float blockrate = xhn::garbage_collector::get()->get_blockrate();
			printf("blockrate %f\n", blockrate);
		}
    }
	while(1) {}
	return 0;
}