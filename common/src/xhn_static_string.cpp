#include "pch.h"
#include "xhn_static_string.hpp"

xhn::hash_set<xhn::string> xhn::static_string::s_static_string_set;

void xhn::static_string::dest()
{
    s_static_string_set.clear();
}