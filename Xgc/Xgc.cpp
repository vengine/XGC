// Xgc.cpp : Defines the entry point for the console application.
//

#include "xhn_garbage_collector.hpp"
#include "xhn_mem_handle.hpp"

int main(void)
{
	int count = 0;
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
		count++;
		if (count > 10000)
		{
			printf("running!!!\n");
			count = 0;
		}
	}
	
	return 0;
}