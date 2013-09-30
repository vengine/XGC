#ifndef XHN_GARBAGE_COLLECTOR_H
#define XHN_GARBAGE_COLLECTOR_H
#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "xhn_mem_map.hpp"
#include "xhn_lock.hpp"
#include "../../framework/include/robot.h"
#include "../../framework/include/robot_thread.h"
#define COMMAND_SENDER "MemoryCommandSender"
#define COMMAND_RECEIVER "MemoryCommandReceiver"
namespace xhn
{
class mem_create_command : public RobotCommand
{
    DeclareRTTI;
public:
	vptr mem;
	euint size;
public:
    mem_create_command(vptr m, euint s)
		: mem(m)
		, size(s)
	{}
	virtual bool Test(Robot* exeRob) { return true; }
	virtual void Do(Robot* exeRob, xhn::static_string sender);
};
class mem_attatch_command : public RobotCommand
{
	DeclareRTTI;
public:
	vptr section;
	vptr mem;
public:
	mem_attatch_command(vptr s, vptr m) 
		: section(s)
		, mem(m)
	{}
	virtual bool Test(Robot* exeRob) { return true; }
	virtual void Do(Robot* exeRob, xhn::static_string sender);
};
class mem_detach_command : public RobotCommand
{
	DeclareRTTI;
public:
	vptr section;
	vptr mem;
public:
	mem_detach_command(vptr s, vptr m) 
		: section(s)
		, mem(m)
	{}
	virtual bool Test(Robot* exeRob) { return true; }
	virtual void Do(Robot* exeRob, xhn::static_string sender);
};

class scan_mem_node_action : public Action
{
	DeclareRTTI;
public:
	virtual void DoImpl();
};

class garbage_collect_robot : public Robot
{
	DeclareRTTI;
public:
	static garbage_collect_robot* s_garbage_collect_robot;
public:
	mem_btree m_btree;
	mem_btree_node* head;
	mem_btree_node* tail;
	euint m_count;
public:
    garbage_collect_robot()
		: m_count(0)
		, head(NULL)
		, tail(NULL)
	{
		s_garbage_collect_robot = this;
	}
	~garbage_collect_robot() 
	{
	}
	static garbage_collect_robot* get() {
		return s_garbage_collect_robot;
	}
	void insert ( const vptr p, euint s ) {
		m_btree.insert((vptr)p, s);
	}
	void remove ( const vptr p ) {
		if (m_btree.remove((vptr)p)) {
			Mfree(p);
		}
		/**
		m_count++;
		if (m_count > 10000)
		    printf("size %d\n", m_btree.size());
		**/
	}
	void attach ( const vptr section, vptr mem ) {
		mem_btree_node* node = m_btree.find(mem);
		if (node) {
			mem_btree_node* parent = m_btree.find(section);
			if (parent) {
				parent->Attach(node);
			}
			else {
				node->AttchToRoot();
			}
		}
	}
	void detach ( const vptr section, vptr mem ) {
		mem_btree_node* node = m_btree.find(mem);
		if (node) {
			mem_btree_node* parent = m_btree.find(section);
			if (parent) {
				parent->Detach(node);
			}
			else {
				node->DetachFromRoot();
			}
		}
	}
	void push_detach_node(mem_btree_node* node) {
		if (node == tail)
			return;
		if (node == head) { head = node->next; }
		if (node->prev) { node->prev->next = node->next; }
		if (node->next) { node->next->prev = node->prev; }
		node->next = NULL;
		node->prev = tail;
		tail = node;
		if (!head)
			head = node;
	}
	void scan_detach_nodes() {
		mem_btree_node* node = tail;
		while (node) {
			node->TrackBack();
			node = node->prev;
		}
	}
	virtual void CommandProcImpl(xhn::static_string sender, RobotCommand* command);
	virtual void CommandReceiptProcImpl(xhn::static_string sender, RobotCommandReceipt* receipt) {}
	virtual xhn::static_string GetName() { return COMMAND_RECEIVER; }
};

class sender_robot : public Robot
{
    DeclareRTTI;
public:
	void create ( const vptr mem, euint size );
	void attach ( const vptr section, vptr mem );
	void detach ( const vptr section, vptr mem );
	virtual xhn::static_string GetName() { return COMMAND_SENDER; }
};

class garbage_collector : public MemObject
{
	template <typename> friend class MemHandle;
	friend class mem_btree_node;
public:
    template <typename T>
    class MemHandle : public BannedAllocObject
    {
        friend class garbage_collector;
    private:
        T* m_ptr;
        bool m_isTransfer;
        explicit MemHandle(T* ptr)
		: m_ptr(ptr)
        , m_isTransfer(true)
        {
        }
        MemHandle(const MemHandle<T>& ptr)
		: m_ptr((T*)ptr.m_ptr)
        , m_isTransfer(false)
        {
            if (m_ptr == ptr.m_ptr)
                return;
            if (m_ptr) {
                garbage_collector::get()->detach((vptr)this, (vptr)m_ptr);
            }
            m_ptr = ptr.m_ptr;
            garbage_collector::get()->attach((vptr)this, (vptr)ptr.m_ptr);
        }
    public:
        explicit MemHandle()
        : m_ptr(NULL)
        , m_isTransfer(false)
        {}
        ~MemHandle()
        {
            if (m_isTransfer) {
                return;
            }
            if (m_ptr) {
                garbage_collector::get()->detach((vptr)this, (vptr)m_ptr);
            }
            m_ptr = NULL;
        }
        const MemHandle& operator = (const MemHandle& ptr) {
            if (m_ptr == ptr.m_ptr)
                return *this;
            if (m_ptr) {
                garbage_collector::get()->detach((vptr)this, (vptr)m_ptr);
            }
            m_ptr = ptr.m_ptr;
			if (ptr.m_ptr)
                garbage_collector::get()->attach((vptr)this, (vptr)ptr.m_ptr);
            return *this;
        }
        T* operator ->() {
            return m_ptr;
        }
        T* get() {
            return m_ptr;
        }
        T& operator *() {
            return *m_ptr;
        }
    };
private:
	static garbage_collector* s_garbage_collector;
private:
	SpinLock m_senderLock;
	sender_robot* m_sender;
private:
    static void* garbage_collector_proc(void* gc);
private:
	garbage_collector();
	~garbage_collector();
	vptr alloc(euint size, const char* _file, euint32 _line);
	void attach(vptr section, vptr mem);
	void detach(vptr section, vptr mem);
public:
	static garbage_collector* get();
    template <typename T>
    MemHandle<T> alloc(const char* _file, euint32 _line) {
		vptr ptr = alloc(sizeof(T), _file, _line);
		new(ptr) T();
		return MemHandle<T>((T*)ptr);
	}
};
}

#define GC_ALLOC(t) xhn::garbage_collector::get()->alloc<t>(__FILE__, __LINE__)
#endif