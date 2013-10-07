/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#include "robot_thread.h"
#include <pthread.h>
///#include "render_system.h"
void* RobotThread::Execute(void *_robThd)
{
    int tmp = 0;
    vptr addr = (vptr)&tmp;
    int ret;
    size_t size;
	RobotThread* robThd = (RobotThread*)_robThd;
    ret = pthread_attr_getstacksize(&robThd->m_attr, &size);
    robThd->m_stackRange.begin_addr = addr;
    robThd->m_stackRange.size = size;
	robThd->Run();
    if (robThd->m_stackRange.begin_addr) {
        Mfree(robThd->m_stackRange.begin_addr);
    }
	delete robThd;
	return NULL;
}

#include <vector>
#include <string>

void RobotThread::Run()
{
    /**
	while (m_isRunning) {
		RobotManager::RobotArray::CheckoutHandle handle;
		Robot* nextActivedRobot = NULL;
		/// loop begin
		do
		{
			nextActivedRobot = NULL;
			/// checkout robot array
			handle = RobotManager::Get()->Checkout();

			xhn::list<Robot*>* robots = handle.Get();
			if (m_curtActivedRobot) {
				robots->push_back(m_curtActivedRobot);
			}
			if (robots->size()) {
				nextActivedRobot = robots->front();
				robots->pop_front();
			}
		} while (!RobotManager::Get()->Submit(handle));

		m_curtActivedRobot = nextActivedRobot;
		if (m_curtActivedRobot) {
			m_curtActivedRobot->DoAction();
		}
	}
     **/
    while (m_isRunning) {
		if (m_isPolling) {
			RobotManager::RobotArray::CheckoutHandle handle;
			Robot* nextActivedRobot = RobotManager::Get()->Pop();
			m_curtActivedRobot = nextActivedRobot;
			if (m_curtActivedRobot) {
				m_curtActivedRobot->DoAction();
				RobotManager::Get()->Push(m_curtActivedRobot);
			}
		}
		else {
			m_curtActivedRobot->DoAction();
		}
		/*
        if (m_sleepNanosecond) {
            struct timespec t;
            t.tv_nsec = m_sleepNanosecond;
            t.tv_sec = 0;
            nanosleep(&t, NULL);
        }
		*/
	}
	m_owner->ThreadStoped(this);
}

bool RobotThread::IsThisThread()
{
    int tmp = 0;
    vptr addr = &tmp;
    bool ret = m_stackRange.IsInStackRange(addr);
    return ret;
    /**
    pthread_t t = pthread_self();
    ///return t == m_thread;
    bool ret2 = (t == m_thread);
    if (ret2)
        return true;
    else
        return false;
    **/
}

RobotThreadManager* RobotThreadManager::s_RobotThreadManager = NULL;
void RobotThreadManager::Init()
{
	s_RobotThreadManager = ENEW RobotThreadManager;
}
RobotThreadManager* RobotThreadManager::Get()
{
	return s_RobotThreadManager;
}

void RobotThreadManager::ThreadStoped(RobotThread* thread)
{
    xhn::RWLock::Instance inst = m_lock.GetWriteLock();
	m_stopedRobotThreads.push_back(thread);
}

RobotThread* RobotThreadManager::AddRobotThread()
{
	xhn::RWLock::Instance inst = m_lock.GetWriteLock();
	RobotThread* robThd = ENEW RobotThread(this, m_threadCount);
	pthread_attr_init(&robThd->m_attr);
	pthread_attr_setdetachstate(&robThd->m_attr, PTHREAD_CREATE_DETACHED);
    int ret;
    int stackSize = 1024 * 1024 * 1024;
    ret = pthread_attr_setstacksize(&robThd->m_attr, stackSize);
	pthread_create(&robThd->m_thread,
                   &robThd->m_attr,
                   RobotThread::Execute,
                   (void *) robThd);
    
	///pthread_attr_destroy(&attr);
	m_activedRobotThreads.push_back(robThd);
	m_threadCount++;
    return robThd;
}

RobotThread* RobotThreadManager::AddRobotThread(Robot* rob)
{
	xhn::RWLock::Instance inst = m_lock.GetWriteLock();
	RobotThread* robThd = ENEW RobotThread(this, rob, m_threadCount);
	pthread_attr_init(&robThd->m_attr);
	pthread_attr_setdetachstate(&robThd->m_attr, PTHREAD_CREATE_DETACHED);
    int ret;
    int stackSize = 1024 * 1024 * 1024;
    ret = pthread_attr_setstacksize(&robThd->m_attr, stackSize);
	pthread_create(&robThd->m_thread,
		           &robThd->m_attr,
		           RobotThread::Execute,
		           (void *) robThd);
	///pthread_attr_destroy(&attr);
	m_activedRobotThreads.push_back(robThd);
	m_threadCount++;
    return robThd;
}

bool RobotThreadManager::IsAllStoped()
{
	xhn::RWLock::Instance inst = m_lock.GetWriteLock();
    return m_stopedRobotThreads.size() == m_threadCount;
}

void RobotThreadManager::StopAllRobotThread()
{
	xhn::RWLock::Instance inst = m_lock.GetWriteLock();
	xhn::list<RobotThread*>::iterator iter = m_activedRobotThreads.begin();
	xhn::list<RobotThread*>::iterator end = m_activedRobotThreads.end();
    for (; iter != end;)
	{
		(*iter)->Stop();
		iter = m_activedRobotThreads.remove(iter);
	}
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