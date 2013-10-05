/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#pragma once
#ifdef __cplusplus
#include "common.h"
#include "etypes.h"
#if defined(USE_OPENGL)
#ifndef __APPLE__
#include "GL/wglew.h"
#include "GL/glew.h"
#endif
#endif
#include "array.h"
#include "comparison_algorithm.h"
#include "container.h"
#include "eassert.h"
#include "elog.h"
#include "emem.h"
#include "emem.hpp"
#include "estring.h"
#include "exception.h"
#include "golden_cudgel.h"
#include "hash.h"
#include "list.h"
#include "path.h"
#include "rwbuffer.h"
#include "stack.h"
#include "tree.h"
#include "map.hpp"
#include "xhn_vector.hpp"
#include "xhn_map.hpp"
#include "xhn_string.hpp"
#include "xhn_hash_set.hpp"
#include "xhn_static_string.hpp"
#endif

/**
 * Copyright (c) 2011-2013 Xu Haining
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */