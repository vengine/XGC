#ifndef XHN_MEM_HANDLE_HPP
#define XHN_MEM_HANDLE_HPP

#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "emem.hpp"
#include <new>

#include "xhn_mem_map.hpp"
#include "xhn_garbage_collector.hpp"

namespace xhn
{
typedef garbage_collector::MemHandle<int> IntHandle;
typedef garbage_collector::MemHandle<float> FloatHandle;
struct TestObject
{
	IntHandle handle0;
    IntHandle handle1;
	FloatHandle handle2;
    garbage_collector::MemHandle<TestObject> hh;
    ~TestObject() {
        printf("here\n");
    }
};
typedef garbage_collector::MemHandle<TestObject> TestObjectHandle;
}

#endif