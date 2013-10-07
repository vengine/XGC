/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef ROBOT_THREAD_H
#define ROBOT_THREAD_H
#include "xhn_static_string.hpp"
#include "xhn_list.hpp"
#include "xhn_vector.hpp"
#include "xhn_smart_ptr.hpp"
#include "robot.h"
#include "emem.hpp"
#include "xhn_lock.hpp"
struct ThreadStackRange
{
    vptr begin_addr;
    euint size;
    ThreadStackRange()
    : begin_addr(NULL)
    , size(0)
    {}
    bool IsInStackRange(vptr addr) {
        /// 这是递减堆栈的算法
        return (ref_ptr)addr <= (ref_ptr)begin_addr &&
        (ref_ptr)addr >= (ref_ptr)begin_addr - (ref_ptr)size;
    }
};
class RobotThread;
class RobotThreadManager : public MemObject
{
	friend class RobotThread;
private:
	static RobotThreadManager* s_RobotThreadManager;
private:
	xhn::RWLock m_lock;
	xhn::list<RobotThread*> m_activedRobotThreads;
	xhn::list<RobotThread*> m_stopedRobotThreads;
	int m_threadCount;
private:
	void ThreadStoped(RobotThread* thread);
public:
	RobotThreadManager()
		: m_threadCount(0)
	{}
	RobotThread* AddRobotThread();
	RobotThread* AddRobotThread(Robot* rob);
	void StopAllRobotThread();
	bool IsAllStoped();
	static void Init();
	static RobotThreadManager* Get();
};

class RobotThread : public MemObject
{
public:
	pthread_t m_thread;
    pthread_attr_t m_attr;
	xhn::SmartPtr< xhn::list<xhn::static_string> > m_dispatchQueue;
	Robot* m_curtActivedRobot;
	bool m_isRunning;
	bool m_isPolling;
	RobotThreadManager* m_owner;
    ThreadStackRange m_stackRange;
	int m_id;
    long m_sleepNanosecond;
public:
	RobotThread(RobotThreadManager* owner, int id)
		: m_curtActivedRobot(NULL)
		, m_isRunning(true)
		, m_isPolling(true)
		, m_owner(owner)
		, m_id(id)
        , m_sleepNanosecond(0)
	{}
	RobotThread(RobotThreadManager* owner, Robot* rob, int id)
		: m_curtActivedRobot(rob)
		, m_isRunning(true)
		, m_isPolling(false)
		, m_owner(owner)
		, m_id(id)
        , m_sleepNanosecond(0)
	{}
	static void* Execute(void *_robThd);
	void Run();
	inline void Stop() {
		m_isRunning = false;
	}
    bool IsThisThread();
    inline void SetSleepNanoSecond(long nanosecond) {
        m_sleepNanosecond = nanosecond;
    }
};
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