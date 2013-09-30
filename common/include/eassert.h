#ifndef EASSERT_H
#define EASSERT_H
#include "common.h"
#include "etypes.h"
#include <assert.h>
API_EXPORT void assert_break();
#define EAssert(e, fmt,...) if (!(e)) { snprintf(g_elog_buffer,4094,fmt,##__VA_ARGS__); assert_break(); ELog_write(); assert(e); }
#endif
