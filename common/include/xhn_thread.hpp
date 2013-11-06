#ifndef XHN_THREAD_HPP
#define XHN_THREAD_HPP
#include "common.h"
#include "etypes.h"
#include "xhn_smart_ptr.hpp"
#include "xhn_list.hpp"
#include "xhn_string.hpp"
#include "xhn_lock.hpp"
namespace xhn
{
class thread : public RefObject 
{
public:
    static void micro_sleep(euint32 microsecond);
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
    volatile bool m_is_errored;
    xhn::string m_error_message;
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
    bool is_errored() {
        return m_is_errored;
    }
    const xhn::string& get_error_message() {
        return m_error_message;
    }
};
typedef SmartPtr<thread> thread_ptr;
}
#endif