// Xgc.cpp : Defines the entry point for the console application.
//

#include "xhn_garbage_collector.hpp"
#include "xhn_mem_handle.hpp"
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
int main(void)
{
    xhn::IntHandle intHandle;
    intHandle = GC_ALLOC(int);
    *intHandle = 0;
	///int count = 0;
	while (1)
	{
		xhn::TestObjectHandle handle;
		handle = GC_ALLOC(xhn::TestObject);
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
        list->PushBack(1);
		list->PushBack(2);
        list->PushBack(3);
		list->PushBack(4);
		iter = list->Begin();
        iter = iter->Next();
		iter->Erase();
		list->Print();
        (*intHandle)++;
        printf("count %d\n", *intHandle);
	}
	while(1) {
		
	}
	return 0;
}