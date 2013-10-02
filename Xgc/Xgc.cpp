// Xgc.cpp : Defines the entry point for the console application.
//

#include "xhn_garbage_collector.hpp"
#include "xhn_mem_handle.hpp"
class MemHandleListenerXX : public xhn::garbage_collector::MemHandleListener
{
public:
    virtual void PreAssign() {
        printf("here\n");
    }
    virtual void PreDest() {
        printf("here\n");
    }
};
static MemHandleListenerXX s_MemHandleListenerXX;
class ListNode
{
public:
    MemHandleListenerXX m_list;
    xhn::garbage_collector::MemHandle<ListNode> m_prev;
	xhn::garbage_collector::MemHandle<ListNode> m_next;
	int m_value;
	ListNode()
	{
        //m_prev = GC_ALLOC(ListNode, "PrevNode");
        ///m_next = GC_ALLOC(ListNode, "NextNode");
        ///m_next.SetListener(&m_list);
    }
	void Erase() {
		if (m_next.get()) { m_next->m_prev = m_prev; }
		if (m_prev.get()) { m_prev->m_next = m_next; }
	}
};
static const char* s_names[] =
    {"PushedNode_value_1",
    "PushedNode_value_2",
    "PushedNode_value_3",
    "PushedNode_value_4"};
typedef xhn::garbage_collector::MemHandle<ListNode> ListNodeHandle;
class List
{
public:
	ListNodeHandle m_head;
	ListNodeHandle m_tail;
public:
	void PushBack(int value) {
        char mbuf[256];
        ///snprintf(mbuf, 255, "PushedNode_value_%d", value);
        ///xhn::static_string str(mbuf);
        ListNodeHandle node;
		node = GC_ALLOC(ListNode, s_names[value - 1]);
		node->m_value = value;
		if (m_tail.get()) {
            m_tail->m_next = node;
			m_tail = node;
		}
		else {
			m_head = m_tail = node;
            ///printf("%llx\n", m_head->m_next.get());
		}
	}
	void Test() {
		ListNodeHandle node;
		node = m_head;
		while(node.get()) {
			node = node->m_next;
			EAssert(((ref_ptr)node.get() & (ref_ptr)0xf0000000) == 0, "error");
		}
	}
	void Print() {
		ListNodeHandle node;
		node = m_head;
		while(node.get()) {
			///printf("##%d\n", node->m_value);
			node = node->m_next;
		}
	}
};
typedef xhn::garbage_collector::MemHandle<List> ListHandle;
int main(void)
{
	///int count = 0;
	while (1)
	{
		/**
		xhn::TestObjectHandle handle;
		handle = GC_ALLOC(xhn::TestObject);
		handle->handle0 = GC_ALLOC(int);
		handle->handle1 = GC_ALLOC(int);
		*handle->handle0 = 100;
		*handle->handle1 = 200;
		handle->handle2 = GC_ALLOC(float);
		*handle->handle2 = (float)(*handle->handle0 + *handle->handle1);
		handle->hh = handle;
		count++;
		if (count > 10000)
		{
			printf("running!!!\n");
			count = 0;
		}
		**/
        ///printf("\n############\n");
		ListHandle list;
        ///list.SetListener(&s_MemHandleListenerXX);
		ListNodeHandle iter;
		list = GC_ALLOC(List, "TestedList");
		list->Test();
		list->PushBack(1);
		list->Test();
		list->PushBack(2);
		list->Test();
        list->PushBack(3);
		list->Test();
		list->PushBack(4);
		list->Test();
		iter = list->m_head;
		list->Test();
		iter = iter->m_next;
		list->Test();
		///iter->Erase();
		list->Print();
		list->Test();
/**
		for (int i = 0; i < 100; i++) {
			xhn::garbage_collect_robot::get()->DoAction();
		}
 **/
        
	}
	while(1) {
		
	}
	return 0;
}