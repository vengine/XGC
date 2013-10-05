// Xgc.cpp : Defines the entry point for the console application.
//

#include "xhn_garbage_collector.hpp"
#include "xhn_mem_handle.hpp"
class ListNode
{
public:
    xhn::garbage_collector::mem_handle<ListNode> m_prev;
	xhn::garbage_collector::mem_handle<ListNode> m_next;
	int m_value;
	ListNode()
	{
    }
	void Erase() {
		if (m_next) { m_next->m_prev = m_prev; }
		if (m_prev) { m_prev->m_next = m_next; }
	}
};

typedef xhn::garbage_collector::mem_handle<ListNode> ListNodeHandle;
class List
{
public:
	ListNodeHandle m_head;
	ListNodeHandle m_tail;
public:
	void PushBack(int value) {
        ///snprintf(mbuf, 255, "PushedNode_value_%d", value);
        ///xhn::static_string str(mbuf);
        ListNodeHandle node;
		node = GC_ALLOC(ListNode);
		node->m_value = value;
		if (m_tail) {
            m_tail->m_next = node;
			m_tail = node;
		}
		else {
			m_head = m_tail = node;
            ///printf("%llx\n", m_head->m_next.get());
		}
	}
    const ListNodeHandle& Begin() {
        return m_head;
    }
	void Print() {
		ListNodeHandle node;
		node = m_head;
		while(node) {
			///printf("##%d\n", node->m_value);
			node = node->m_next;
		}
	}
};
typedef xhn::garbage_collector::mem_handle<List> ListHandle;
int main(void)
{
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
		list = GC_ALLOC(List);
        list->PushBack(1);
		list->PushBack(2);
        list->PushBack(3);
		list->PushBack(4);
		iter = list->Begin();
		iter->Erase();
		list->Print();
	}
	while(1) {
		
	}
	return 0;
}