/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef ROBOT_H
#define ROBOT_H
///**********************************************************************///
///                           include begin                              ///
///**********************************************************************///
#include "common.h"
#include "etypes.h"
#include "xhn_vector.hpp"
#include "xhn_set.hpp"
#include "xhn_map.hpp"
#include "xhn_static_string.hpp"
#include "xhn_lock.hpp"
#include "emem.hpp"
#include "xhn_smart_ptr.hpp"
#include "rtti.hpp"
#include "rwbuffer.h"
#include "timer.h"

///#define USE_COMMAND_PTR
///**********************************************************************///
///                           include end                                ///
///**********************************************************************///
///**********************************************************************///
///                       class define begin                             ///
///**********************************************************************///

class SafedBuffer : public MemObject
{
public:
	char* m_transferBuffer;
	RWBuffer m_buffer;
	euint64 m_nonblockingCount;
	euint64 m_blockingCount;
public:
	SafedBuffer(euint bufferSize);
	~SafedBuffer();
	template <typename T>
	void Write(const T& from) {
		while (!RWBuffer_Write(m_buffer, (const euint*)&from, sizeof(T))) {
			m_blockingCount++;
		}
		m_nonblockingCount++;
	}
	void Write(const void* buf, euint size);
	char* Read(euint* readSize);
	inline bool IsEmpty() {
		return RWBuffer_IsEmpty(m_buffer);
	}
	inline float GetBlockrate()
	{
		float ret = (float)((double)m_blockingCount / (double)(m_blockingCount + m_nonblockingCount));
		m_blockingCount = 0;
		m_nonblockingCount = 0;
		return ret;
	}
};

class Robot;
class Action : public RefObject
{
	DeclareRootRTTI;
public:
	TimeCheckpoint m_prevTimeCheckPoint;
	double m_lastProcessingTime;
public:
	Action()
		: m_lastProcessingTime(0.0)
	{}
	virtual ~Action() {}
	virtual void DoImpl() = 0;
	void Do();
	void Tick();
    void Tock();
	inline double GetLastProcessingTime() {
		return m_lastProcessingTime;
	}
};
typedef xhn::SmartPtr<Action> ActionPtr;
///**********************************************************************///
///                       class define end                               ///
///**********************************************************************///
///**********************************************************************///
///                       class define begin                             ///
///**********************************************************************///
class RobotCommandBase : public MemObject
{
    DeclareRootRTTI;
};

class RobotCommand : public RobotCommandBase
{
	DeclareRTTI;
public:
	virtual ~RobotCommand() {}
	virtual bool Test(Robot* exeRob) = 0;
	virtual void Do(Robot* exeRob, xhn::static_string sender) = 0;
};

class RobotCommandReceipt : public RobotCommandBase
{
    DeclareRTTI;
public:
	virtual ~RobotCommandReceipt() {}
	virtual var ShowDetails() = 0;
};
///**********************************************************************///
///                       class define end                               ///
///**********************************************************************///
///**********************************************************************///
///                       class define begin                             ///
///**********************************************************************///
class Robot : public MemObject
{
    DeclareRootRTTI;
public:
	///pthread_t m_thread;
    typedef xhn::vector<ActionPtr> ActionQueue;
    typedef xhn::map<xhn::static_string, SafedBuffer*> ChannelMap;
	ActionQueue m_actionQueue;
	int m_curtActionIndex;
private:
	ChannelMap m_commandReceivingChannels;
	ChannelMap m_commandTransmissionChannels;
public:
	Robot()
		: m_curtActionIndex(-1)
	{}
	virtual ~Robot();
	inline void AddAction(ActionPtr act) {
		 m_actionQueue.push_back(act);
	}
	friend class RobotManager;
public:
	void RunOnce();
	void CommandProc();
	virtual bool CommandProcImpl(xhn::static_string sender, RobotCommand* command) { return true; }
	virtual bool CommandReceiptProcImpl(xhn::static_string sender, RobotCommandReceipt* receipt) { return true; }
	virtual xhn::static_string GetName() = 0;
	virtual void InitChannels() {}
	inline void Next() {
		if (m_actionQueue.size()) {
			m_curtActionIndex++;
			if ((euint)m_curtActionIndex >= m_actionQueue.size())
				m_curtActionIndex = 0;
		}
		else
			m_curtActionIndex = -1;
	}
	inline ActionPtr GetCurrnetAction() {
		ActionPtr ret;
		if (m_actionQueue.size()) {
			if (m_curtActionIndex < 0)
				m_curtActionIndex = 0;
			if ((euint)m_curtActionIndex >= m_actionQueue.size())
				m_curtActionIndex = 0;
			ret = m_actionQueue[m_curtActionIndex];
		}
		return ret;
	}
	virtual void DoAction();
};
///**********************************************************************///
///                       class define end                               ///
///**********************************************************************///
///**********************************************************************///
///                       class define begin                             ///
///**********************************************************************///
class RobotManager : public MemObject
{
private:
    static RobotManager* s_RobotManager;
public:
    xhn::RWLock2 m_readwriteLock;
	typedef xhn::SmartPtr< xhn::list<Robot*> > RobotArray;
    typedef xhn::map<xhn::static_string, Robot*> RobotMap;
	RobotArray m_robots;
	RobotMap m_robotMap;
private:
	RobotManager();
	~RobotManager();
public:
	template<typename R>
	R* AddRobot() {
		R* rob = ENEW R;
		{
			xhn::RWLock2::Instance inst =  m_readwriteLock.GetWriteLock();
			m_robots->push_back((Robot*)rob);
			m_robotMap.insert(xhn::make_pair(rob->GetName(), (Robot*)rob));
		}
		rob->InitChannels();
		return rob;
	}
	template<typename R, typename P0>
	R* AddRobot(P0 p0) {
		R* rob = ENEW R(p0);
		{
			xhn::RWLock2::Instance inst =  m_readwriteLock.GetWriteLock();
			m_robots->push_back((Robot*)rob);
			m_robotMap.insert(xhn::make_pair(rob->GetName(), (Robot*)rob));
		}
		rob->InitChannels();
		return rob;
	}
    Robot* GetRobot(xhn::static_string robName);
	void MakeChannel(xhn::static_string sender, xhn::static_string receiver);
	///void BreakChannel(xhn::static_string sender, xhn::static_string receiver);
	SafedBuffer* GetChannel(xhn::static_string sender, xhn::static_string receiver);
	static void Init();
	static void Dest();
	static RobotManager* Get();
	inline RobotArray::CheckoutHandle Checkout() {
		xhn::RWLock2::Instance inst = m_readwriteLock.GetReadLock();
		return m_robots.Checkout();
	}
	inline bool Submit(RobotArray::CheckoutHandle& handle) {
		xhn::FGarbageCollectProc< xhn::list<Robot*> > gc;
		bool ret = false;
		{
			xhn::RWLock2::Instance inst = m_readwriteLock.GetWriteLock();
			ret = m_robots.Submit(handle, gc);
		}
		return ret;
	}
    /// remove a robot, who is no longer be controlled by a manager, but it also in m_robotMap
    Robot* Remove(xhn::static_string robName);
	void DeleteAllRobots();
    Robot* Pop();
    void Push(Robot* rob);
	///void SendCommand(xhn::static_string sender, xhn::static_string receiver, RobotCommand* cmd);
	///void SendCommand(SafedBuffer* channel, RobotCommand* cmd);
};
///**********************************************************************///
///                       class define end                               ///
///**********************************************************************///
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
