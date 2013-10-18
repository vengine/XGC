#ifndef XHN_THREAD_HPP
#define XHN_THREAD_HPP
#include "common.h"
#include "etypes.h"
#include "xhn_smart_ptr.hpp"
#include "xhn_list.hpp"
#include "xhn_lock.hpp"
namespace xhn
{
class thread : public RefObject 
{
public:
	class task : public RefObject
	{
	public:
		virtual ~task() {} 
		virtual void run() = 0;
	};
	typedef SmartPtr<task> task_ptr;
public:
	SpinLock m_lock;
	list<task_ptr> m_tasks;
	volatile bool m_is_finished;
	volatile bool m_is_stopped;
	volatile bool m_is_completed;
private:
	static void* thread_proc(void* self);
public:
    thread();
	~thread();
	void run();
	void add_task(task_ptr t);
	bool is_complete() {
		return m_is_completed;
	}
};
typedef SmartPtr<thread> thread_ptr;
}
#endif