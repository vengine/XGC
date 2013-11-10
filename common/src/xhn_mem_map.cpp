/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "pch.h"
#include "xhn_mem_map.hpp"
#include "xhn_garbage_collector.hpp"

void xhn::mem_btree_node::Attach(const vptr handle, mem_btree_node* mem)
{
	output_map.insert(xhn::make_pair(handle, mem));
	input_pair ip = {this, (vptr)handle};
	mem->input_set.insert(ip);
}
void xhn::mem_btree_node::Detach(const vptr handle, mem_btree_node* mem)
{
	output_map.erase(handle);
	input_pair ip = {this, (vptr)handle};
	mem->input_set.erase(ip);
	garbage_collect_robot::get()->push_detach_node(mem);
}
void xhn::mem_btree_node::AttchToRoot() {
	root_ref_count++;
}
void xhn::mem_btree_node::DetachFromRoot() {
	if (!root_ref_count)
		return;
	root_ref_count--;
	///EAssert(root_ref_count >= 0, "count must greater or equal zero");
}
bool xhn::mem_btree_node::_TrackBack(mem_set& trackBuffer)
{
    if (root_ref_count)
        return true;
    
    {
        input_mem_set::iterator iter = input_set.begin();
        input_mem_set::iterator end = input_set.end();
        for (; iter != end; iter++)
        {
            input_pair& ip = *iter;
            mem_btree_node* i = ip.node;
            if (this == i) {
                continue;
            }
            if (trackBuffer.find(i) != trackBuffer.end()) {
                continue;
            }
            trackBuffer.insert(i);
            if (i->_TrackBack(trackBuffer)) {
                return true;
            }
        }
    }
    return false;
}

void xhn::mem_btree_node::_Erase()
{
    mem_map::iterator iter = output_map.begin();
    mem_map::iterator end = output_map.end();
    for (; iter != end; iter++) {
        vptr handle = iter->first;
        mem_btree_node* i = iter->second;
        input_pair ip = {this, handle};
        if (this == i)
            continue;
        i->input_set.erase(ip);
        ///i->TrackBack();
        ///garbage_collect_robot::get()->push_detach_node(i);
    }
    output_map.clear();
}

bool xhn::mem_btree_node::TrackBack()
{
    mem_set trackBuffer;
    return _TrackBack(trackBuffer);
}

void xhn::mem_btree_node::MarkNotGarbage()
{
	if (!is_garbage)
		return;
	is_garbage = false;
	mem_map::iterator iter = output_map.begin();
	mem_map::iterator end = output_map.end();
	for (; iter != end; iter++) {
        mem_btree_node* node = iter->second;
		node->MarkNotGarbage();
		node->is_garbage = false;
	}
}
void xhn::mem_btree::push_detach_node(mem_btree_node* node)
{
    garbage_collect_robot::get()->push_detach_node(node);
}

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