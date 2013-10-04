#include "pch.h"
#include "xhn_garbage_collector.hpp"

ImplementRTTI(xhn::mem_create_command, RobotCommand);
ImplementRTTI(xhn::mem_attatch_command, RobotCommand);
ImplementRTTI(xhn::mem_detach_command, RobotCommand);
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

void xhn::scan_mem_node_action::DoImpl()
{
	garbage_collect_robot::get()->scan_detach_nodes();
}

void xhn::garbage_collect_robot::CommandProcImpl(xhn::static_string sender, RobotCommand* command)
{
    command->Do(this, sender);
}

void xhn::sender_robot::create ( const vptr mem, euint size, const char* name, destructor dest )
{
	mem_create_command* cmd = ENEW mem_create_command(mem, size, name, dest);
	RobotManager::Get()->SendCommand(GetName(), COMMAND_RECEIVER, cmd);
}
void xhn::sender_robot::attach ( const vptr section, vptr mem )
{
    mem_attatch_command* cmd = ENEW mem_attatch_command(section, mem);
	RobotManager::Get()->SendCommand(GetName(), COMMAND_RECEIVER, cmd);
}
void xhn::sender_robot::detach ( const vptr section, vptr mem )
{
	mem_detach_command* cmd = ENEW mem_detach_command(section, mem);
	RobotManager::Get()->SendCommand(GetName(), COMMAND_RECEIVER, cmd);
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

	scan_mem_node_action* act = ENEW scan_mem_node_action;

	garbage_collect_robot* gc_rob = RobotManager::Get()->AddRobot<garbage_collect_robot>();
	m_sender = RobotManager::Get()->AddRobot<sender_robot>();
	gc_rob->AddAction(act);

	RobotManager::Get()->MakeChannel(COMMAND_SENDER, COMMAND_RECEIVER);

    if (!s_garbage_collect_robot_thread) {
	    RobotThreadManager::Init();
	    s_garbage_collect_robot_thread = RobotThreadManager::Get()->AddRobotThread();
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
    vptr ret = SMalloc(size);
	if (ret) {
		SpinLock::Instance inst = m_senderLock.Lock();
		m_sender->create(ret, size, _name, dest);
	}
	else {
		printf("here\n");
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