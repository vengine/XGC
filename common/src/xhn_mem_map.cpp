#include "pch.h"
#include "xhn_mem_map.hpp"
#include "xhn_garbage_collector.hpp"

bool xhn::mem_btree_node::_TrackBack(mem_set& trackBuffer)
{
	if (root_ref_count)
		return true;

	mem_set::iterator iter = input_set.begin();
	mem_set::iterator end = input_set.end();
	for (; iter != end; iter++)
	{
		mem_btree_node* i = *iter;
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
	{
		iter = output_set.begin();
		end = output_set.end();
		for (; iter != end; iter++) {
			mem_btree_node* i = *iter;
			if (this == i)
				continue;
			i->input_set.erase(this);
			///i->TrackBack();
			garbage_collect_robot::get()->push_detach_node(i);
		}
		output_set.clear();
		{
			iter = input_set.begin();
			end = input_set.end();
			for (; iter != end; iter++)
			{
				mem_btree_node* i = *iter;
				i->output_set.erase(this);
			}
			input_set.clear();
			garbage_collect_robot::get()->remove(begin_addr);
		}
	}
	return false;
}

bool xhn::mem_btree_node::TrackBack()
{
	mem_set trackBuffer;
	return _TrackBack(trackBuffer);
}

void xhn::mem_btree_node::Attach(mem_btree_node* mem)
{
	mem->input_set.insert(this);
	output_set.insert(mem);
}
void xhn::mem_btree_node::Detach(mem_btree_node* mem)
{
	output_set.erase(mem);
	mem->input_set.erase(this);
	garbage_collect_robot::get()->push_detach_node(mem);
}
void xhn::mem_btree_node::AttchToRoot() {
	AtomicIncrement(&root_ref_count);
}
void xhn::mem_btree_node::DetachFromRoot() {
	AtomicDecrement(&root_ref_count);
	EAssert(root_ref_count >= 0, "count must greater or equal zero");
	///TrackBack();
	garbage_collect_robot::get()->push_detach_node(this);
}