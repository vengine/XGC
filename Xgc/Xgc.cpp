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
		node = GC_ALLOC(ListNode, NULL);
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

struct TTT
{
    euint32 mbuf[3];
    ///char mmm[3];
    ///char mbuf[12];
};

typedef xhn::garbage_collector::mem_handle<List> ListHandle;
int main(void)
{
    TTT t0;
    TTT t1;
    TTT t2;
    TTT t3;
    euint s;
    
    t0.mbuf[0] = 100000;
    t0.mbuf[1] = 200000;
    t0.mbuf[2] = 300000;
    
    /**
    t0.mmm[0] = 'a';
    t0.mmm[1] = 'b';
    t0.mmm[2] = 0x00;
     **/
    ///memcpy(t0.mbuf, "abcdefghijk", strlen("abcdefghijk") + 1);
    RWBuffer buffer = RWBuffer_new(256);
    
    RWBuffer_Write(buffer, (const euint*)&t0, sizeof(t0));
    RWBuffer_Write(buffer, (const euint*)&t0, sizeof(t0));
    RWBuffer_Read(buffer, (euint*)&t1, &s);
    RWBuffer_Read(buffer, (euint*)&t2, &s);
    RWBuffer_Write(buffer, (const euint*)&t0, sizeof(t0));
    RWBuffer_Read(buffer, (euint*)&t3, &s);
    ///printf("%s\n%s\n%s\n", t1.mbuf, t2.mbuf, t2.mbuf);
    xhn::IntHandle intHandle;
    intHandle = GC_ALLOC(int, "intHandle");
    *intHandle = 0;
	///int count = 0;
	while (1)
	///for (int i = 0; i < 100; i++)
	{
		xhn::TestObjectHandle handle;
		handle = GC_ALLOC(xhn::TestObject, "TestObject");
		handle->handle0 = GC_ALLOC(int, "handle0");
		handle->handle1 = GC_ALLOC(int, "handle1");
		*handle->handle0 = 100;
		*handle->handle1 = 200;
		handle->handle2 = GC_ALLOC(float, "handle2");
		*handle->handle2 = (float)(*handle->handle0 + *handle->handle1);
		handle->hh = handle;

		ListHandle list;
		ListNodeHandle iter;
        GC_ALLOC(List, "List0");
		list = GC_ALLOC(List, "List");
		for (int i = 0; i < 100; i++) {
			list->PushBack(i);
		}
		///list->Print();
        (*intHandle)++;
		printf("count %d\n", *intHandle);
		/**
		if (*intHandle > 1000) {
		    printf("count %d\n", *intHandle);
			*intHandle = 0;
		}
		**/
		///Sleep(1);
    }
	while(1) {}
	return 0;
}