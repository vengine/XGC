#include "pch.h"
#include "xhn_mem_map.hpp"
#include "xhn_garbage_collector.hpp"

bool xhn::mem_btree_node::_TrackBack(mem_set& trackBuffer)
{
	if (root_ref_count)
		return true;

    {
        input_mem_map::iterator iter = input_map.begin();
        input_mem_map::iterator end = input_map.end();
        for (; iter != end; iter++)
        {
            input_pair& ip = iter->first;
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
	{
		mem_map::iterator iter = output_map.begin();
		mem_map::iterator end = output_map.end();
		for (; iter != end; iter++) {
            vptr handle = iter->first;
			mem_btree_node* i = iter->second;
            input_pair ip = {this, handle};
			if (this == i)
				continue;
			i->input_map.erase(ip);
			///i->TrackBack();
			///garbage_collect_robot::get()->push_detach_node(i);
		}
		output_map.clear();
	}
    {
        ///garbage_collect_robot::get()->remove(begin_addr);
    }
	return false;
}

bool xhn::mem_btree_node::TrackBack()
{
	mem_set trackBuffer;
	return _TrackBack(trackBuffer);
}

void xhn::mem_btree_node::Attach(const vptr handle, mem_btree_node* mem)
{
    input_pair ip = {this, (vptr)handle};
	mem->input_map.insert(xhn::make_pair(ip, mem));
	output_map.insert(xhn::make_pair(handle, mem));
}
void xhn::mem_btree_node::Detach(const vptr handle, mem_btree_node* mem)
{
    input_pair ip = {this, (vptr)handle};
	output_map.erase(handle);
	mem->input_map.erase(ip);
	garbage_collect_robot::get()->push_detach_node(mem);
}
void xhn::mem_btree_node::AttchToRoot() {
	AtomicIncrement(&root_ref_count);
}
void xhn::mem_btree_node::DetachFromRoot() {
	AtomicDecrement(&root_ref_count);
	EAssert(root_ref_count >= 0, "count must greater or equal zero");
	///TrackBack();
}

void xhn::mem_btree::push_detach_node(mem_btree_node* node)
{
    garbage_collect_robot::get()->push_detach_node(node);
}