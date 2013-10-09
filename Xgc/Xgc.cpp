// Xgc.cpp : Defines the entry point for the console application.
//

#include "xhn_garbage_collector.hpp"
#include "xhn_mem_handle.hpp"
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

static const char* sss[] = {
	"Value0",
	"Value1",
	"Value2",
	"Value3",
	"Value4"
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
	int* ttt = (int*)Malloc(sizeof(int));
	int* ttt1 = (int*)Malloc(sizeof(int));
    xhn::IntHandle intHandle;
    intHandle = GC_ALLOC(int);
    *intHandle = 0;
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
		
		for (int i = 0; i < 100; i++) {
			list->PushBack(i);
		}
		///list->Print();
        (*intHandle)++;
		printf("count %d\n", *intHandle);
		/**
		if (*intHandle > 1000) {
			Sleep(100);
			*intHandle = 0;
		}
		**/
    }
	while(1) {}
	return 0;
}