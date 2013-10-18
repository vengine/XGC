#include "xhn_thread.hpp"

void* xhn::thread::thread_proc(void* self)
{
    thread* t = (thread*)self;
	t->run();
	return NULL;
}

void xhn::thread::run()
{
	while (1) {
		if (m_is_finished)
			break;
		if (m_tasks.empty()) {
			SleepEx(1, FALSE);
			continue;
		}
        task_ptr t;
		{
			SpinLock::Instance inst = m_lock.Lock();
			if (m_tasks.size()) {
				t = m_tasks.front();
				m_tasks.pop_front();
			}
		}
		if (t.get()) {
			t->run();
			if (m_tasks.empty()) {
				m_is_completed = true;
			}
		}
	}
	m_is_stopped = true;
}

void xhn::thread::add_task(task_ptr t)
{
    SpinLock::Instance inst = m_lock.Lock();
	m_tasks.push_back(t);
	m_is_completed = false;
}

xhn::thread::thread()
: m_is_finished(false)
, m_is_stopped(false)
, m_is_completed(false)
{
	pthread_t pt;
	///pthread_attr_t attr;
	///pthread_attr_init(&attr);
	///pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	///pthread_attr_setstacksize(&attr, 1024);
	pthread_create(&pt,
		NULL,
		thread_proc,
		(void *) this);
	///pthread_attr_destroy(&attr);
}
xhn::thread::~thread()
{
	m_is_finished = true;
	while (!m_is_stopped) {}
}