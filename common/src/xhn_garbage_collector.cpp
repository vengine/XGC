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
ImplementRTTI(xhn::scan_mem_node_action, Action);
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
	{
        garbage_collect_robot::get()->scan_orphan_nodes(garbage_collect_robot::get()->command_count);
        garbage_collect_robot::get()->command_count = 0;
	}
}

void xhn::scan_mem_node_action::DoImpl()
{
	{
		garbage_collect_robot::get()->scan_detach_nodes();
	}
}

void xhn::garbage_collect_robot::CommandProcImpl(xhn::static_string sender, RobotCommand* command)
{
    command->Do(this, sender);
    command_count++;
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
	while (!m_channel->Write(cmd)) {
		m_blockingCount++;
	}
	m_nonblockingCount++;
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
	while (!m_channel->Write(cmd)) {
		m_blockingCount++;
	}
	m_nonblockingCount++;
#endif
}
void xhn::sender_robot::detach ( const vptr section, vptr mem )
{
    if (!m_channel) {
        m_channel = RobotManager::Get()->GetChannel(GetName(), COMMAND_RECEIVER);
    }
#ifdef USE_COMMAND_PTR
	mem_detach_command* cmd = ENEW mem_detach_command(section, mem);
	while (!m_channel->Write(cmd));
#else
	mem_detach_command cmd(section, mem);
	while (!m_channel->Write(cmd)) {
		m_blockingCount++;
	}
	m_nonblockingCount++;
#endif
}
float xhn::sender_robot::get_blockrate()
{
    float ret = (float)((double)m_blockingCount / (double)(m_blockingCount + m_nonblockingCount));
	m_blockingCount = 0;
	m_nonblockingCount = 0;
	return ret;
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
	scan_mem_node_action* smna = ENEW scan_mem_node_action;

	garbage_collect_robot* gc_rob = RobotManager::Get()->AddRobot<garbage_collect_robot>();
	m_sender = RobotManager::Get()->AddRobot<sender_robot>();
    gc_rob->AddAction(sona);
	gc_rob->AddAction(smna);

	RobotManager::Get()->MakeChannel(COMMAND_SENDER, COMMAND_RECEIVER);

    if (!s_garbage_collect_robot_thread) {
	    RobotThreadManager::Init();
	    s_garbage_collect_robot_thread = RobotThreadManager::Get()->AddRobotThread();
        s_garbage_collect_robot_thread->SetSleepNanoSecond(0);
    }
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
		SpinLock::Instance inst = m_senderLock.Lock();
		m_sender->create(ret, size, _name, dest);
	}
	return ret;
}

void xhn::garbage_collector::attach(vptr section, vptr mem)
{
	SpinLock::Instance inst = m_senderLock.Lock();
    m_sender->attach(section, mem);
}

void xhn::garbage_collector::detach(vptr section, vptr mem)
{
	SpinLock::Instance inst = m_senderLock.Lock();
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