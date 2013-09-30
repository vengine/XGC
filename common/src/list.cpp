#include "pch.h"
#include "list.h"
#include "container.h"
#include "spin_lock.h"

#define ALLOW_CONCURRENT
#define ListFunc(s) List_##s
#include "list_base.h"
