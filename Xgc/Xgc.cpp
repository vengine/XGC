// Xgc.cpp : Defines the entry point for the console application.
//

#include "xhn_garbage_collector.hpp"
#include "rwbuffer.h"
#include "timer.h"
#include "xhn_fixed_queue.hpp"
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

void test0() 
{
	IntHandle intHandle;
	intHandle = GC_ALLOC(int);
	*intHandle = 0;
	TimeCheckpoint tc = TimeCheckpoint::Tick();
	while(1) {
		TestObjectHandle testHandle0;
		TestObjectHandle testHandle1;

		testHandle0 = GC_ALLOC(TestObject);

		testHandle0->handle0 = GC_ALLOC(int);
		testHandle0->handle1 = GC_ALLOC(int);
		{
			FloatHandle floatTest0;
			FloatHandle floatTest1;
			FloatHandle floatTest2;

			floatTest0 = GC_ALLOC(float);
			floatTest1 = GC_ALLOC(float);
			floatTest2 = GC_ALLOC(float);
			*floatTest0 = 32.0f;
			*floatTest1 = 24.0f;
			*floatTest2 = *floatTest0 * *floatTest1;
			testHandle0->handle2 = floatTest2;

			TestObjectHandle ttt;
			ttt = GC_ALLOC(TestObject);
			ttt->hh = GC_ALLOC(TestObject);
			testHandle0->hh = ttt;
		}

		testHandle1 = GC_ALLOC(TestObject);

		testHandle1->handle0 = GC_ALLOC(int);
		testHandle1->handle1 = GC_ALLOC(int);
		testHandle1->hh = GC_ALLOC(TestObject);

		testHandle1->hh->handle0 = GC_ALLOC(int);
		testHandle1->hh->handle1 = GC_ALLOC(int);

		testHandle1->hh->hh = GC_ALLOC(TestObject);

		testHandle0->hh = testHandle1;
		testHandle1->hh = testHandle0;

		(*intHandle)++;

		if ((*intHandle % 10000) == 0) {
			float blockrate = xhn::garbage_collector::get()->get_blockrate();
			TimeCheckpoint curtTc = TimeCheckpoint::Tick();
			double t = TimeCheckpoint::CaleElapsedTime(tc, curtTc);
			printf("blockrate %f time %f, %f ops per second\n", blockrate, (float)t, 150000.0f / (float)(t / 1000.0 / 1000.0));
			tc = TimeCheckpoint::Tick();
		}
	}
}

void test1()
{
	IntHandle intHandle;
	intHandle = GC_ALLOC(int);
	*intHandle = 0;
	TimeCheckpoint tc = TimeCheckpoint::Tick();
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
			TimeCheckpoint curtTc = TimeCheckpoint::Tick();
			double t = TimeCheckpoint::CaleElapsedTime(tc, curtTc);
			printf("blockrate %f time %f, %f ops per second\n", blockrate, (float)t, 100000.0f / (float)(t / 1000.0 / 1000.0));
			tc = TimeCheckpoint::Tick();
		}
	}
}

typedef xhn::garbage_collector::array<int> IntArray;
typedef xhn::garbage_collector::array<TestObject> TestArray;

void test2()
{
	IntHandle intHandle;
	intHandle = GC_ALLOC(int);
	*intHandle = 0;
	while (1)
	{
		IntArray intArray;
		intArray = GC_ALLOC_ARRAY(int, 100);
		intArray[0] = GC_ALLOC(int);
		intArray[1] = GC_ALLOC(int);
		intArray[2] = GC_ALLOC(int);
		intArray[0] = 100;
		intArray[1] = 200;
		intArray[2] = 100;

		TestArray testArray;
		testArray = GC_ALLOC_ARRAY(TestObject, 5);
		for (euint i = 0; i < testArray.size(); i++) {
			testArray[0].handle0 = GC_ALLOC(int);
			*testArray[0].handle0 = 100;
		}

		(*intHandle)++;

		if ((*intHandle % 10000) == 0) {
			float blockrate = xhn::garbage_collector::get()->get_blockrate();
			printf("blockrate %f\n", blockrate);
		}
	}
}

void test3()
{
	int ttt = 0;
	TimeCheckpoint tc = TimeCheckpoint::Tick();
	while (1)
	{
		for (int i = 0; i < 150; i++) {
		    TestObject* aaa = new TestObject;
			delete aaa;
	    }
		ttt++;

		if ((ttt % 10000) == 0) {
			TimeCheckpoint curtTc = TimeCheckpoint::Tick();
			double t = TimeCheckpoint::CaleElapsedTime(tc, curtTc);
			printf("%f, %f ops per second\n", (float)t, 1500000.0f / (float)(t / 1000.0 / 1000.0));
			tc = TimeCheckpoint::Tick();
		}
	}
}

void test4()
{
    /**
    int num = 1000000;
    xhn::vector<vptr> ptrs;
    xhn::mem_btree tree;
    xhn::set<vptr> ptr_set;
    xhn::mem_map2<vptr, euint64> mem_map;
    ptrs.resize(num);
    for (int i = 0; i < num; i++) {
        ptrs[i] = (vptr)rand();
    }
    
    {
        TimeCheckpoint tc = TimeCheckpoint::Tick();
        for (int i = 0; i < num; i++) {
            mem_map.insert(ptrs[i], (vptr)((ref_ptr)ptrs[i] + 1), 0);
        }
        TimeCheckpoint curtTc = TimeCheckpoint::Tick();
        double t = TimeCheckpoint::CaleElapsedTime(tc, curtTc);
        printf("%f\n", (float)t);
    }
    
    {
        TimeCheckpoint tc = TimeCheckpoint::Tick();
        for (int i = 0; i < num; i++) {
            ptr_set.insert(ptrs[i]);
        }
        TimeCheckpoint curtTc = TimeCheckpoint::Tick();
        double t = TimeCheckpoint::CaleElapsedTime(tc, curtTc);
        printf("%f\n", (float)t);
    }
    
    {
        TimeCheckpoint tc = TimeCheckpoint::Tick();
        for (int i = 0; i < num; i++) {
            tree.insert(ptrs[i], 1, NULL, NULL);
        }
        TimeCheckpoint curtTc = TimeCheckpoint::Tick();
        double t = TimeCheckpoint::CaleElapsedTime(tc, curtTc);
        printf("%f\n", (float)t);
    }
     **/
}

void test5()
{
    /**
    xhn::mem_map2<euint64, euint64> mem_map;
    mem_map.insert(0, 200, 0);
    mem_map.insert(205, 300, 0);
    mem_map.insert(1205, 1300, 0);
    mem_map.insert(1400, 1500, 0);
    mem_map.insert(2400, 2500, 0);
    
    xhn::mem_map2<euint64, euint64>::node_ptr ret = mem_map.find(300);
    mem_map.remove(ret);
    if (ret)
        printf("succ\n");
    else
        printf("fail\n");
    ret = mem_map.find(300);
    if (ret)
        printf("succ\n");
    else
        printf("fail\n");
     **/
}

struct TTT
{
	char mbuf0[6];
	char mbuf1[4];
};

int main(void)
{
	test0();
    getchar();
	return 0;
}