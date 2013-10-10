/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "robot.h"

ImplementRootRTTI(Action);
ImplementRootRTTI(RobotCommandBase);
ImplementRTTI(RobotCommand, RobotCommandBase);
ImplementRTTI(RobotCommandReceipt, RobotCommandBase);
ImplementRootRTTI(Robot);
///**********************************************************************///
///                       class implement begin                          ///
///**********************************************************************///
void Action::Do()
{
	Tick();
	DoImpl();
	Tock();
}
void Action::Tick()
{
	m_prevTimeCheckPoint = TimeCheckpoint::Tick();
}
void Action::Tock()
{
	TimeCheckpoint tcp = TimeCheckpoint::Tick();
	m_lastProcessingTime =
    TimeCheckpoint::CaleElapsedTime(m_prevTimeCheckPoint, tcp);
    
	m_prevTimeCheckPoint = tcp;
}
///**********************************************************************///
///                       class implement end                            ///
///**********************************************************************///
///**********************************************************************///
///                       class implement begin                          ///
///**********************************************************************///
Robot::~Robot()
{
	ChannelMap::iterator iter = m_commandTransmissionChannels.begin();
	ChannelMap::iterator end = m_commandTransmissionChannels.end();
	for (; iter != end; iter++) {
		SafedBuffer* buf = iter->second;
		delete buf;
	}
}
void Robot::RunOnce()
{
	ActionQueue::iterator iter = m_actionQueue.begin();
	ActionQueue::iterator end = m_actionQueue.end();
	for (; iter != end; iter++) {
		ActionPtr act = *iter;
		act->Do();
		CommandProc();
	}
	CommandProc();
}

void Robot::CommandProc()
{
	ChannelMap::iterator iter = m_commandReceivingChannels.begin();
    ChannelMap::iterator end = m_commandReceivingChannels.end();
    for (; iter != end; iter++)
	{
		euint size = 0;
		SafedBuffer* channel = iter->second;
		char* ret = NULL;
		while ( (ret = channel->Read(&size)) )
		///if ( (ret = channel->Read(&size)) )
		{
#ifdef USE_COMMAND_PTR
			RobotCommandBase* rcb = *((RobotCommandBase**)ret);
			RobotCommand* cmd =
				rcb->DynamicCast<RobotCommand>();
			if (cmd) {
				CommandProcImpl(iter->first, cmd);
				delete rcb;
				continue;
			}
			RobotCommandReceipt* rec =
				rcb->DynamicCast<RobotCommandReceipt>();
			if (rec) {
				CommandReceiptProcImpl(iter->first, rec);
				delete rcb;
			}
#else
			RobotCommandBase* rcb = (RobotCommandBase*)ret;
			RobotCommand* cmd =
            rcb->DynamicCast<RobotCommand>();
			if (cmd) {
				if (!CommandProcImpl(iter->first, cmd))
					break;
				continue;
			}
			RobotCommandReceipt* rec =
			rcb->DynamicCast<RobotCommandReceipt>();
			if (rec) {
				if (!CommandReceiptProcImpl(iter->first, rec))
					break;
			}
#endif
		}
	}
}
///**********************************************************************///
///                       class implement end                            ///
///**********************************************************************///
///**********************************************************************///
///                       class implement begin                          ///
///**********************************************************************///
RobotManager* RobotManager::s_RobotManager = NULL;
RobotManager::RobotManager()
{
	m_robots = ENEW xhn::list<Robot*>;
}
RobotManager::~RobotManager()
{
}

Robot* RobotManager::GetRobot(xhn::static_string robName)
{
    xhn::RWLock2::Instance inst = m_readwriteLock.GetReadLock();
    RobotMap::iterator iter = m_robotMap.find(robName);
    if (iter != m_robotMap.end()) {
        return iter->second;
    }
    else
        return NULL;
}

void RobotManager::MakeChannel(xhn::static_string sender,
                               xhn::static_string receiver)
{
    xhn::RWLock2::Instance inst = m_readwriteLock.GetWriteLock();
	RobotMap::iterator s = m_robotMap.find(sender);
	if (s == m_robotMap.end())
		return;
	RobotMap::iterator r = m_robotMap.find(receiver);
    if (r == m_robotMap.end())
		return;
	//////////////////////////////////////////////////////////////////////////
	Robot* sRob = s->second;
    Robot::ChannelMap::iterator iter =
    sRob->m_commandTransmissionChannels.find(receiver);
	SafedBuffer* channel = NULL;
	if (iter != sRob->m_commandTransmissionChannels.end())
		channel = iter->second;
	else {
		channel = ENEW SafedBuffer(128 * 1024);
		sRob->m_commandTransmissionChannels.insert(
            xhn::make_pair(receiver, channel)
        );
	}
	//////////////////////////////////////////////////////////////////////////
	Robot* rRob = r->second;
	iter = rRob->m_commandReceivingChannels.find(sender);
    if (iter != rRob->m_commandReceivingChannels.end())
		return;
	else
		rRob->m_commandReceivingChannels.insert(
            xhn::make_pair(sender, channel)
        );
}
/**
void RobotManager::BreakChannel(xhn::static_string sender,
                                xhn::static_string receiver)
{
	xhn::RWLock2::Instance inst = m_readwriteLock.GetWriteLock();
	RobotMap::iterator r = m_robotMap.find(receiver);
	if (r != m_robotMap.end()) {
		Robot* rRob = r->second;
		rRob->m_commandReceivingChannels.erase(sender);
	}
	RobotMap::iterator s = m_robotMap.find(sender);
	if (s != m_robotMap.end()) {
		Robot* sRob = r->second;
		Robot::ChannelMap::iterator iter =
        sRob->m_commandTransmissionChannels.find(receiver);
        
		if (iter != sRob->m_commandTransmissionChannels.end()) {
			RobotCommand* cmd[2];
			euint size = 0;
			RWBuffer channel = iter->second;
			while (RWBuffer_Read(channel, (euint*)cmd, &size)) {
				if (size >= sizeof(RobotCommand*))
					delete cmd[0];
			}
			RWBuffer_delete(iter->second);
		}
	}
}
**/
SafedBuffer* RobotManager::GetChannel(xhn::static_string sender,
                                  xhn::static_string receiver)
{
    xhn::RWLock2::Instance inst = m_readwriteLock.GetReadLock();
	RobotMap::iterator s = m_robotMap.find(sender);
	if (s == m_robotMap.end())
		return NULL;
	Robot* sRob = s->second;
    
	Robot::ChannelMap::iterator iter =
    sRob->m_commandTransmissionChannels.find(receiver);
    
	if (iter != sRob->m_commandTransmissionChannels.end())
		return iter->second;
	else
		return NULL;
}

void RobotManager::Init()
{
	if (!s_RobotManager)
		s_RobotManager = ENEW RobotManager;
}
void RobotManager::Dest()
{
	if (s_RobotManager) {
		delete s_RobotManager;
		s_RobotManager = NULL;
	}
}
RobotManager* RobotManager::Get()
{
	return s_RobotManager;
}
Robot* RobotManager::Remove(xhn::static_string robName)
{
    xhn::RWLock2::Instance inst =  m_readwriteLock.GetWriteLock();

    xhn::list< Robot*>::iterator iter = m_robots->begin();
	xhn::list< Robot*>::iterator end = m_robots->end();
	for (; iter != end; iter++) {
		if ((*iter)->GetName() == robName) {
			Robot* ret = *iter;
			m_robots->remove(iter);
			return ret;
		}
	}
	return NULL;
}
void RobotManager::DeleteAllRobots()
{
	xhn::RWLock2::Instance inst =  m_readwriteLock.GetWriteLock();

	xhn::list< Robot*>::iterator iter = m_robots->begin();
	xhn::list< Robot*>::iterator end = m_robots->end();
	for (; iter != end; ) {
		Robot* rob = *iter;
		delete rob;
		iter = m_robots->remove(iter);
	}
}
Robot* RobotManager::Pop()
{
    xhn::RWLock2::Instance inst =  m_readwriteLock.GetWriteLock();
    Robot* ret = NULL;
    if (m_robots->size()) {
        ret = m_robots->front();
        m_robots->pop_front();
    }
    return ret;
}
void RobotManager::Push(Robot* rob)
{
    xhn::RWLock2::Instance inst =  m_readwriteLock.GetWriteLock();
    m_robots->push_back(rob);
}
/**
void RobotManager::SendCommand(xhn::static_string sender, xhn::static_string receiver, RobotCommand* cmd)
{
	SafedBuffer* channel =
		RobotManager::Get()->GetChannel(sender,
		receiver);
	if (channel) {
		SendCommand(channel, cmd);
	}
	else
		delete cmd;
}
void RobotManager::SendCommand(SafedBuffer* channel, RobotCommand* cmd)
{
     ///while (!RWBuffer_Write(channel, (const euint*)&cmd, sizeof(cmd))) {}
	while (!channel->Write(cmd)) {}
}
**/
///**********************************************************************///
///                       class implement end                            ///
///**********************************************************************///

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