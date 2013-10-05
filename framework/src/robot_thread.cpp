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
	RobotThread* robThd = (RobotThread*)_robThd;
	robThd->Run();
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
	}
	m_owner->ThreadStoped(this);
}

bool RobotThread::IsThisThread()
{
    pthread_t t = pthread_self();
    return t == m_thread;
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
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, 1024);
	pthread_create(&robThd->m_thread,
                   NULL,
                   RobotThread::Execute,
                   (void *) robThd);
	pthread_attr_destroy(&attr);
	m_activedRobotThreads.push_back(robThd);
	m_threadCount++;
    return robThd;
}

RobotThread* RobotThreadManager::AddRobotThread(Robot* rob)
{
	xhn::RWLock::Instance inst = m_lock.GetWriteLock();
	RobotThread* robThd = ENEW RobotThread(this, rob, m_threadCount);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, 1024);
	pthread_create(&robThd->m_thread,
		NULL,
		RobotThread::Execute,
		(void *) robThd);
	pthread_attr_destroy(&attr);
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