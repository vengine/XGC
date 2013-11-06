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
#include "xhn_garbage_collector.hpp"

ImplementRTTI(xhn::mem_create_command, RobotCommand);
ImplementRTTI(xhn::mem_attatch_command, RobotCommand);
ImplementRTTI(xhn::mem_detach_command, RobotCommand);
ImplementRTTI(xhn::scan_orphan_node_action, Action);
ImplementRTTI(xhn::collect_action, Action);
ImplementRTTI(xhn::track_action, Action);
ImplementRTTI(xhn::garbage_collect_robot, Robot);
ImplementRTTI(xhn::sender_robot, Robot);

xhn::garbage_collect_robot* xhn::garbage_collect_robot::s_garbage_collect_robot = NULL;

void xhn::mem_create_command::Do(Robot* exeRob, xhn::static_string sender)
{
    garbage_collect_robot* gcRobot = exeRob->DynamicCast<garbage_collect_robot>();
	gcRobot->insert(mem, size, name, dest);
}
void xhn::mem_attatch_command::Do(Robot* exeRob, xhn::static_string sender)
{
    garbage_collect_robot* gcRobot = exeRob->DynamicCast<garbage_collect_robot>();
	gcRobot->attach(section, mem);
}
void xhn::mem_detach_command::Do(Robot* exeRob, xhn::static_string sender)
{
	garbage_collect_robot* gcRobot = exeRob->DynamicCast<garbage_collect_robot>();
	gcRobot->detach(section, mem);
}

void xhn::scan_orphan_node_action::DoImpl()
{	
    garbage_collect_robot::get()->scan_orphan_nodes(garbage_collect_robot::get()->command_count);
    garbage_collect_robot::get()->command_count = 0;
}

void xhn::collect_action::DoImpl()
{
	garbage_collect_robot::get()->pre_collect();
	garbage_collect_robot::get()->mark_not_garbage();
	garbage_collect_robot::get()->collect();
}

void xhn::track_action::DoImpl()
{
    garbage_collect_robot::get()->track();
}

bool xhn::garbage_collect_robot::CommandProcImpl(xhn::static_string sender, RobotCommand* command)
{
    command->Do(this, sender);
    command_count++;
	if (!command->DynamicCast<mem_detach_command>())
		return true;
	else
		return false;
}

void xhn::sender_robot::create ( const vptr mem, euint size, const char* name, destructor dest )
{
    if (!m_channel) {
        m_channel = RobotManager::Get()->GetChannel(GetName(), COMMAND_RECEIVER);
    }
#ifdef USE_COMMAND_PTR
	mem_create_command* cmd = ENEW mem_create_command(mem, size, name, dest);
    while (!m_channel->Write(cmd));
#else
	mem_create_command cmd(mem, size, name, dest);
	m_channel->Write(cmd);
#endif
#ifdef GC_DEBUG
	garbage_collect_robot::get()->DoAction();
	garbage_collect_robot::get()->DoAction();
#endif
}
void xhn::sender_robot::attach ( const vptr section, vptr mem )
{
    if (!m_channel) {
        m_channel = RobotManager::Get()->GetChannel(GetName(), COMMAND_RECEIVER);
    }
#ifdef USE_COMMAND_PTR
	mem_attatch_command* cmd = ENEW mem_attatch_command(section, mem);
	while (!m_channel->Write(cmd));
#else
    mem_attatch_command cmd(section, mem);
	m_channel->Write(cmd);
#endif
#ifdef GC_DEBUG
	garbage_collect_robot::get()->DoAction();
	garbage_collect_robot::get()->DoAction();
#endif
}
void xhn::sender_robot::detach ( const vptr section, vptr mem )
{
    if (!m_channel) {
        m_channel = RobotManager::Get()->GetChannel(GetName(), COMMAND_RECEIVER);
    }
#ifdef USE_COMMAND_PTR
	mem_detach_command* cmd = ENEW mem_detach_command(section, mem);
	m_channel->Write(cmd);
#else
	mem_detach_command cmd(section, mem);
	m_channel->Write(cmd);
#endif
#ifdef GC_DEBUG
	garbage_collect_robot::get()->DoAction();
	garbage_collect_robot::get()->DoAction();
#endif
}
float xhn::sender_robot::get_blockrate()
{
    if (m_channel)
        return m_channel->GetBlockrate();
    else
        return 0.0f;
}

void xhn::garbage_collect_robot::DoAction()
{
	ActionPtr act = GetCurrnetAction();
	for (int i = 0; i < 5; i++) CommandProc();
	if (act.get()) act->Do();
	for (int i = 0; i < 5; i++) CommandProc();
	Next();
}

vptr xhn::garbage_collect_robot::alloc ( euint size ) {
	vptr ret = Malloc(size);
#ifdef PRINT_ALLOC_INFO
#if BIT_WIDTH == 32 
	printf("alloc %x\n", (ref_ptr)ret);
#else
	printf("alloc %llx\n", (ref_ptr)ret);
#endif
#endif
	return ret;
}
xhn::mem_btree_node* xhn::garbage_collect_robot::find_node_in_queue(vptr mem) {
	xhn::fixed_queue<mem_info>::iterator iter = m_mem_info_queue.begin();
	xhn::fixed_queue<mem_info>::iterator end = m_mem_info_queue.end();
	for (; iter != end; iter++) {
		mem_info& info = *iter;
		if ((ref_ptr)mem >= (ref_ptr)info.begin_addr &&
			(ref_ptr)mem <= (ref_ptr)info.end_addr)
			return info.mem_node;
	}
	return NULL;
}
void xhn::garbage_collect_robot::remove_in_queue(vptr mem) {
	xhn::fixed_queue<mem_info>::iterator iter = m_mem_info_queue.begin();
	xhn::fixed_queue<mem_info>::iterator end = m_mem_info_queue.end();
	for (; iter != end; iter++) {
		mem_info& info = *iter;
		if ((ref_ptr)mem >= (ref_ptr)info.begin_addr &&
			(ref_ptr)mem <= (ref_ptr)info.end_addr) 
		{
			info.begin_addr = NULL;
			info.end_addr = NULL;
			info.mem_node = NULL;
			return;
		}
	}
}
void xhn::garbage_collect_robot::insert ( const vptr p, euint s, const char* n, destructor d ) {
	mem_btree_node* node = m_btree.insert((vptr)p, s, n, d);
	node->is_garbage = false;
	push_orphan_node(node);
	mem_info info = {node->begin_addr, node->end_addr, node};
	if (!m_mem_info_queue.push(info)) {
		m_mem_info_queue.pop();
		m_mem_info_queue.push(info);
	}
}
void xhn::garbage_collect_robot::remove ( const vptr p ) {
	remove_in_queue((vptr)p);
	if (m_btree.remove((vptr)p)) {
#ifdef PRINT_ALLOC_INFO
#if BIT_WIDTH == 32 
		printf("free %x\n", (ref_ptr)p);
#else
		printf("free %llx\n", (ref_ptr)p);
#endif
#endif
		Mfree(p);
	}
}
void xhn::garbage_collect_robot::attach ( const vptr section, vptr mem ) {
	mem_btree_node* node = find_node_in_queue(mem);///m_btree.find(mem);
	if (!node) {
		node = (mem_btree_node*)m_btree.find(mem);
	}
	if (node) {
		mem_btree_node* parent = find_node_in_queue(section);///m_btree.find(section);
		if (!parent) {
			parent = (mem_btree_node*)m_btree.find(section);
		}
		if (parent) {
			parent->Attach(section, node);
#ifdef GC_DEBUG
			if (m_isDebugging) {
				printf("##Attach begin##\n");
				if (node->name && parent->name) {
					printf("%s attach to the %s\n", node->name, parent->name);
					printf("%s root ref count is %d\n",
						node->name,
						(euint32)node->root_ref_count);
					printf("%s number of output links is %d\n",
						node->name,
						(euint32)node->output_map.size());
					printf("%s root ref count is %d\n",
						parent->name,
						(euint32)parent->root_ref_count);
					printf("%s number of input links is %d\n",
						parent->name,
						(euint32)parent->input_set.size());
					printf("%s number of output links is %d\n",
						parent->name,
						(euint32)parent->output_map.size());
				}
				else {
#if BIT_WIDTH == 32
					printf("%x attach to the %x\n", 
						(ref_ptr)node->begin_addr, 
						(ref_ptr)parent->begin_addr);
					printf("%x root ref count is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->root_ref_count);
					printf("%x number of input links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->input_set.size());
					printf("%x number of output links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->output_map.size());
					printf("%x root ref count is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->root_ref_count);
					printf("%x number of input links is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->input_set.size());
					printf("%x number of output links is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->output_map.size());
#else
					printf("%llx attach to the %llx\n", 
						(ref_ptr)node->begin_addr, 
						(ref_ptr)parent->begin_addr);
					printf("%llx root ref count is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->root_ref_count);
					printf("%llx number of input links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->input_set.size());
					printf("%llx number of output links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->output_map.size());
					printf("%llx root ref count is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->root_ref_count);
					printf("%llx number of input links is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->input_set.size());
					printf("%llx number of output links is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->output_map.size());
#endif
				}
				printf("##Attach end##\n");
			}
#endif
		}
		else {
			node->AttchToRoot();
#ifdef GC_DEBUG
			if (m_isDebugging) {
				printf("##Attach begin##\n");
				if (node->name) {
					printf("%s attach to root\n", node->name);
					printf("%s root ref count is %d\n",
						node->name,
						(euint32)node->root_ref_count);
					printf("%s number of input links is %d\n",
						node->name,
						(euint32)node->input_set.size());
					printf("%s number of output links is %d\n",
						node->name,
						(euint32)node->output_map.size());
				}
				else {
#if BIT_WIDTH == 32
					printf("%x attach to root\n", 
						(ref_ptr)node->begin_addr);
					printf("%x root ref count is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->root_ref_count);
					printf("%x number of input links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->input_set.size());
					printf("%x number of output links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->output_map.size());
#else
					printf("%llx attach to root\n", 
						(ref_ptr)node->begin_addr);
					printf("%llx root ref count is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->root_ref_count);
					printf("%llx number of input links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->input_set.size());
					printf("%llx number of output links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->output_map.size());
#endif
				}
				printf("##Attach end##\n");
			}
#endif
		}
		push_detach_node(node);
	}
}
void xhn::garbage_collect_robot::detach ( const vptr section, vptr mem ) {
	mem_btree_node* node = find_node_in_queue(mem);///m_btree.find(mem);
	if (!node) {
		node = (mem_btree_node*)m_btree.find(mem);
	}
	if (node) {
		mem_btree_node* parent = find_node_in_queue(section);///m_btree.find(section);
		if (!parent) {
			parent = (mem_btree_node*)m_btree.find(section);
		}
		if (parent) {
			parent->Detach(section, node);
#ifdef GC_DEBUG
			if (m_isDebugging) {
				printf("##Detach begin##\n");
				if (node->name && parent->name) {
					printf("%s detach from the %s\n", node->name, parent->name);
					printf("%s root ref count is %d\n",
						node->name,
						(euint32)node->root_ref_count);
					printf("%s number of input links is %d\n",
						node->name,
						(euint32)node->input_set.size());
					printf("%s number of output links is %d\n",
						node->name,
						(euint32)node->output_map.size());
					printf("%s root ref count is %d\n",
						parent->name,
						(euint32)parent->root_ref_count);
					printf("%s number of input links is %d\n",
						parent->name,
						(euint32)parent->input_set.size());
					printf("%s number of output links is %d\n",
						parent->name,
						(euint32)parent->output_map.size());
				}
				else {
#if BIT_WIDTH == 32
					printf("%x detach from the %x\n", 
						(ref_ptr)node->begin_addr, 
						(ref_ptr)parent->begin_addr);
					printf("%x root ref count is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->root_ref_count);
					printf("%x number of input links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->input_set.size());
					printf("%x number of output links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->output_map.size());
					printf("%x root ref count is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->root_ref_count);
					printf("%x number of input links is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->input_set.size());
					printf("%x number of output links is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->output_map.size());
#else
					printf("%llx detach from the %x\n", 
						(ref_ptr)node->begin_addr, 
						(ref_ptr)parent->begin_addr);
					printf("%llx root ref count is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->root_ref_count);
					printf("%llx number of input links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->input_set.size());
					printf("%llx number of output links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->output_map.size());
					printf("%llx root ref count is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->root_ref_count);
					printf("%llx number of input links is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->input_set.size());
					printf("%llx number of output links is %d\n",
						(ref_ptr)parent->begin_addr,
						(euint32)parent->output_map.size());
#endif
				}
				printf("##Detach end##\n");
			}
#endif
		}
		else {
			node->DetachFromRoot();
#ifdef GC_DEBUG
			if (m_isDebugging) {
				printf("##Detach begin##\n");
				if (node->name) {
					printf("%s detach from root\n", node->name);
					printf("%s root ref count is %d\n",
						node->name,
						(euint32)node->root_ref_count);
					printf("%s number of input links is %d\n",
						node->name,
						(euint32)node->input_set.size());
					printf("%s number of output links is %d\n",
						node->name,
						(euint32)node->output_map.size());
				}
				else {
#if BIT_WIDTH == 32
					printf("%x detach from root\n", (ref_ptr)node->begin_addr);
					printf("%x root ref count is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->root_ref_count);
					printf("%x number of input links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->input_set.size());
					printf("%x number of output links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->output_map.size());
#else
					printf("%llx detach from root\n", (ref_ptr)node->begin_addr);
					printf("%llx root ref count is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->root_ref_count);
					printf("%llx number of input links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->input_set.size());
					printf("%llx number of output links is %d\n",
						(ref_ptr)node->begin_addr,
						(euint32)node->output_map.size());
#endif
				}
				printf("##Detach end##\n");
			}
#endif
		}
	}
}
void xhn::garbage_collect_robot::push_orphan_node(mem_btree_node* node) {
	if (orphan) {
		orphan->prev = node;
	}
	node->next = orphan;
	node->prev = NULL;
	orphan = node;
}
void xhn::garbage_collect_robot::push_detach_node(mem_btree_node* node) {
	if (node == tail)
		return;
	if (node == orphan) { orphan = node->next; }
	if (node == head)   { head = node->next; }
	if (node->prev)     { node->prev->next = node->next; }
	if (node->next)     { node->next->prev = node->prev; }
	node->next = NULL;
	node->prev = tail;
	if (tail)
		tail->next = node;
	tail = node;
	if (!head)
		head = node;

	if (node->root_ref_count) {
		if (node == root_tail)
			return;
		if (node == root_head)   { root_head = node->root_next; }
		if (node->root_prev)     { node->root_prev->root_next = node->root_next; }
		if (node->root_next)     { node->root_next->root_prev = node->root_prev; }
		node->root_next = NULL;
		node->root_prev = root_tail;
		if (root_tail)
			root_tail->root_next = node;
		root_tail = node;
		if (!root_head)
			root_head = node;
	}
}
void xhn::garbage_collect_robot::scan_orphan_nodes(esint32 num_cmds) {
	curt_state = NormalState;
	mem_btree_node* node = orphan;
	while (node) {
		node->orphan_count -= num_cmds;
		if (node->orphan_count <= 0) {
			mem_btree_node* tmp = node->next;
			push_detach_node(node);
			node = tmp;
		}
		else
			node = node->next;
	}
}
void xhn::garbage_collect_robot::erase(mem_btree_node* node) {
	node->_Erase();

	if (node == head) { head = node->next; }
	if (node == tail) { tail = node->prev; }
	if (node->prev)   { node->prev->next = node->next; }
	if (node->next)   { node->next->prev = node->prev; }

	if (node == root_head) { root_head = node->root_next; }
	if (node == root_tail) { root_tail = node->root_prev; }
	if (node->root_prev)   { node->root_prev->root_next = node->root_next; }
	if (node->root_next)   { node->root_next->root_prev = node->root_prev; }
}
void xhn::garbage_collect_robot::pre_collect() {
	if (!all_collect)
	    return;
	curt_state = PreCollectState;
	mem_btree_node* node = tail;
	while (node) {
		node->is_garbage = true;
		node = node->prev;
	}
}
void xhn::garbage_collect_robot::mark_not_garbage() {
	if (!all_collect)
		return;
	curt_state = MarkNotGarbageState;
	mem_btree_node* node = root_tail;
	while (node) {
		if (node->root_ref_count) {
			node->MarkNotGarbage();
		}
		node = node->root_prev;
	}
}
void xhn::garbage_collect_robot::collect() {
	if (!all_collect)
		return;
	curt_state = CollectState;
	mem_btree_node* node = tail;
	while (node) {
		if (node->is_garbage) {
			vptr ptr = node->begin_addr;
			erase(node);
			node = node->prev;
			remove(ptr);
		}
		else {
			node = node->prev;
		}
	}
}
void xhn::garbage_collect_robot::track() {
	if (all_collect) {
		all_collect = false;
		return;
	}
	if (m_btree.get_alloced_size() > 128 * 1024 * 1024) {
		all_collect = true;
		return;
	}
	curt_state = TrackState;

	euint delta_mem = 0;
	if (m_btree.count > prev_alloced_mems)
		delta_mem = m_btree.count - prev_alloced_mems;
	esint count = (esint)(delta_mem * 5);
	prev_alloced_mems = m_btree.count;

	mem_btree_node* node = tail;
	while (node) {
		if (!node->TrackBack()) {
			vptr ptr = node->begin_addr;
			erase(node);
			node = node->prev;
			remove(ptr);
		}
		else {
			node = node->prev;
		}
		count--;
		if (count <= 0)
			break;
	}
}

xhn::garbage_collector* xhn::garbage_collector::s_garbage_collector = NULL;
RobotThread* xhn::garbage_collector::s_garbage_collect_robot_thread = NULL;

void* xhn::garbage_collector::garbage_collector_proc(void* gc)
{
	return NULL;
}

xhn::garbage_collector::garbage_collector()
{
	ELog_Init();

	RobotManager::Init();

    scan_orphan_node_action* sona = ENEW scan_orphan_node_action;
	///scan_mem_node_action* smna = ENEW scan_mem_node_action;
	collect_action* ca = ENEW collect_action;
    track_action* ta = ENEW track_action;

	garbage_collect_robot* gc_rob = RobotManager::Get()->AddRobot<garbage_collect_robot>();
	m_sender = RobotManager::Get()->AddRobot<sender_robot>();
    gc_rob->AddAction(sona);
	///gc_rob->AddAction(smna);
	gc_rob->AddAction(ca);
    gc_rob->AddAction(ta);

	RobotManager::Get()->MakeChannel(COMMAND_SENDER, COMMAND_RECEIVER);
#ifndef GC_DEBUG
    if (!s_garbage_collect_robot_thread) {
	    RobotThreadManager::Init();
	    s_garbage_collect_robot_thread = RobotThreadManager::Get()->AddRobotThread(gc_rob);
        s_garbage_collect_robot_thread->SetSleepNanoSecond(0);
    }
#endif
}
xhn::garbage_collector::~garbage_collector()
{}
vptr xhn::garbage_collector::alloc(euint size,
                                   const char* _file,
                                   euint32 _line,
                                   const char* _name,
                                   destructor dest)
{
    vptr ret = garbage_collect_robot::get()->alloc(size);
	if (ret) {
#ifndef GC_DEBUG
		SpinLock::Instance inst = m_senderLock.Lock();
#endif
		m_sender->create(ret, size, _name, dest);
	}
	return ret;
}

void xhn::garbage_collector::attach(vptr section, vptr mem)
{
#ifndef GC_DEBUG
	SpinLock::Instance inst = m_senderLock.Lock();
#endif
    m_sender->attach(section, mem);
}

void xhn::garbage_collector::detach(vptr section, vptr mem)
{
#ifndef GC_DEBUG
	SpinLock::Instance inst = m_senderLock.Lock();
#endif
	m_sender->detach(section, mem);
}

xhn::garbage_collector* xhn::garbage_collector::get()
{
	if (!s_garbage_collector) {
		s_garbage_collector = ENEW garbage_collector;
	}
	return s_garbage_collector;
}

bool xhn::garbage_collector::is_garbage_collect_robot_thread()
{
    if (s_garbage_collect_robot_thread) {
        return s_garbage_collect_robot_thread->IsThisThread();
    }
    return false;
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