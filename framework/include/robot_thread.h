#ifndef ROBOT_THREAD_H
#define ROBOT_THREAD_H
#include "xhn_static_string.hpp"
#include "xhn_list.hpp"
#include "xhn_vector.hpp"
#include "xhn_smart_ptr.hpp"
#include "robot.h"
#include "emem.hpp"
#include "xhn_lock.hpp"
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
	xhn::SmartPtr< xhn::list<xhn::static_string> > m_dispatchQueue;
	Robot* m_curtActivedRobot;
	bool m_isRunning;
	bool m_isPolling;
	RobotThreadManager* m_owner;
	int m_id;
public:
	RobotThread(RobotThreadManager* owner, int id)
		: m_curtActivedRobot(NULL)
		, m_isRunning(true)
		, m_isPolling(true)
		, m_owner(owner)
		, m_id(id)
	{}
	RobotThread(RobotThreadManager* owner, Robot* rob, int id)
		: m_curtActivedRobot(rob)
		, m_isRunning(true)
		, m_isPolling(false)
		, m_owner(owner)
		, m_id(id)
	{}
	static void* Execute(void *_robThd);
	void Run();
	inline void Stop() {
		m_isRunning = false;
	}
    bool IsThisThread();
};
#endif