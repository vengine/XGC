#include "xhn_vector.hpp"
#include "xhn_list.hpp"
#include "xhn_smart_ptr.hpp"
#include "emem.hpp"
#include "emem2.h"
#include <vector>
#include "elog.h"
#ifndef __APPLE__
class Timer
{
public:

	inline Timer()
	{
		LARGE_INTEGER temp;
		QueryPerformanceFrequency((LARGE_INTEGER*)&temp);
		m_freq = ((double)temp.QuadPart) / 1000.0;
	}
	inline void Start()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
	}

	inline float Stop()
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_endTime);
		float elapsedTime = (float)(((double)m_endTime.QuadPart - (double)m_startTime.QuadPart) / m_freq);
		return elapsedTime;
	}

private:

	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_endTime;

	double m_freq;
};

struct test_struct : public MemObject
{
	int a;
    test_struct()
		: a(0)
	{
	    printf("here\n");
	}
	test_struct(test_struct& s)
		: a(s.a)
	{
		printf("here\n");
	}
};
void VectorTest()
{
	xhn::list<int>* test_list = ENEW xhn::list<int>;
	test_list->push_back(1);
	test_list->push_back(2);
	xhn::SmartPtr< xhn::list<int> > list_ptr = test_list;
	xhn::SmartPtr< xhn::list<int> >::CheckoutHandle ch = list_ptr.Checkout();
	xhn::FGarbageCollectProc< xhn::list<int> > gc;
	while (!list_ptr.Submit(ch, gc))
	{
		ch = list_ptr.Checkout();
	}

	xhn::vector<int> test;
	test.push_back(0);
	test.push_back(1);
	test.push_back(2);
	xhn::vector<int>::iterator iter = test.begin();
	iter++;
	test.insert(iter, 15);
	for (euint i = 0; i < test.size(); i++)
	{
		elog("test %d", test[i]);
	}
	/**
	xhn::vector<test_struct> ttt;
	test_struct tt;
	tt.a = 300;
	ttt.push_back(tt);
	ttt.resize(1000);
	for (euint i = 0; i < ttt.size(); i++)
	{
        test_struct t = ttt[i];
		printf("test_struct.a %d\n", t.a);
	}
	std::vector<int> std_test;
	xhn::vector<int> xhn_test;
	///xhn_test.reserve(10000);
	{
		Timer timer;
		timer.Start();
		for (int i = 0; i < 1000; i++)
		{
			std_test.push_back(i);
		}
        float e = timer.Stop();
		elog("std time %f", e);
	}
	{
		Timer timer;
		timer.Start();
		for (int i = 0; i < 1000; i++)
		{
			xhn_test.push_back(i);
		}
		float e = timer.Stop();
		elog("ecg time %f", e);
	}
	**/
	{
		BigMemoryAlloctor alloc = BigMemoryAlloctor_new(32 * 1024 * 1024);
		/**
		vptr p0 = BigMemoryAlloctor_alloc(alloc, 1000);
		vptr p1 = BigMemoryAlloctor_alloc(alloc, 1000);
		BigMemoryAlloctor_free(p0);
		BigMemoryAlloctor_free(p1);
		p0 = BigMemoryAlloctor_alloc(alloc, 4000);
	    p1 = BigMemoryAlloctor_alloc(alloc, 8000);
		BigMemoryAlloctor_free(p0);
		BigMemoryAlloctor_free(p1);
		**/
#define NUM_POINTERS 256
		vptr ptrs[NUM_POINTERS];
		{
			Timer timer;
		    timer.Start();
			for (int i = 0; i < NUM_POINTERS; i++)
			    ptrs[i] = malloc(i * 4);
			/**
		    for (int i = 0; i < NUM_POINTERS; i++)
			    free(ptrs[i]);
				**/
			float e = timer.Stop();
		    elog("std time %f", e);
		}
		{
			Timer timer;
		    timer.Start();
			for (int i = 0; i < NUM_POINTERS; i++)
			    ptrs[i] = BigMemoryAlloctor_alloc(alloc, i * 4);
			/**
		    for (int i = 0; i < NUM_POINTERS; i++)
			    BigMemoryAlloctor_free(ptrs[i]);
				**/
			float e = timer.Stop();
		    elog("ecg time %f", e);
		}
		{
			Timer timer;
		    timer.Start();
			for (int i = 0; i < NUM_POINTERS; i++)
			    ptrs[i] = Malloc(i * 4);
			/**
		    for (int i = 0; i < NUM_POINTERS; i++)
			    BigMemoryAlloctor_free(ptrs[i]);
				**/
			float e = timer.Stop();
		    elog("mem time %f", e);
		}
		elog("mem alloc");
	}
}
#endif