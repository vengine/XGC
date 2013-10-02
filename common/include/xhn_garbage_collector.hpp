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
    const char* name;
public:
    mem_create_command(vptr m, euint s, const char* n)
    : mem(m)
    , size(s)
    , name(n)
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
    bool m_isDebugging;
public:
    garbage_collect_robot()
    : m_count(0)
    , head(NULL)
    , tail(NULL)
    , m_isDebugging(false)
	{
		s_garbage_collect_robot = this;
	}
	~garbage_collect_robot() 
	{
	}
	static garbage_collect_robot* get() {
		return s_garbage_collect_robot;
	}
	void insert ( const vptr p, euint s, const char* n ) {
		m_btree.insert((vptr)p, s, n);
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
				parent->Attach(section, node);
                if (m_isDebugging) {
                    printf("##Attach begin##\n");
                    printf("%s attach to the %s\n", node->name, parent->name);
                    printf("%s root ref count is %d\n",
                           node->name,
                           (euint32)node->root_ref_count);
                    printf("%s number of input links is %d\n",
                           node->name,
                           (euint32)node->input_map.size());
                    printf("%s number of output links is %d\n",
                           node->name,
                           (euint32)node->output_map.size());
                    printf("%s root ref count is %d\n",
                           parent->name,
                           (euint32)parent->root_ref_count);
                    printf("%s number of input links is %d\n",
                           parent->name,
                           (euint32)parent->input_map.size());
                    printf("%s number of output links is %d\n",
                           parent->name,
                           (euint32)parent->output_map.size());
                    printf("##Attach end##\n");
                }
			}
			else {
				node->AttchToRoot();
                if (m_isDebugging) {
                    printf("##Attach begin##\n");
                    printf("%s attach to root\n", node->name);
                    printf("%s root ref count is %d\n",
                           node->name,
                           (euint32)node->root_ref_count);
                    printf("%s number of input links is %d\n",
                           node->name,
                           (euint32)node->input_map.size());
                    printf("%s number of output links is %d\n",
                           node->name,
                           (euint32)node->output_map.size());
                    printf("##Attach end##\n");
                }
			}
            push_detach_node(node);
		}
	}
	void detach ( const vptr section, vptr mem ) {
		mem_btree_node* node = m_btree.find(mem);
		if (node) {
			mem_btree_node* parent = m_btree.find(section);
			if (parent) {
				parent->Detach(section, node);
                if (m_isDebugging) {
                    printf("##Detach begin##\n");
                    printf("%s detach from the %s\n", node->name, parent->name);
                    printf("%s root ref count is %d\n",
                           node->name,
                           (euint32)node->root_ref_count);
                    printf("%s number of input links is %d\n",
                           node->name,
                           (euint32)node->input_map.size());
                    printf("%s number of output links is %d\n",
                           node->name,
                           (euint32)node->output_map.size());
                    printf("%s root ref count is %d\n",
                           parent->name,
                           (euint32)parent->root_ref_count);
                    printf("%s number of input links is %d\n",
                           parent->name,
                           (euint32)parent->input_map.size());
                    printf("%s number of output links is %d\n",
                           parent->name,
                           (euint32)parent->output_map.size());
                    printf("##Detach end##\n");
                }
			}
			else {
				node->DetachFromRoot();
                if (m_isDebugging) {
                    printf("##Detach begin##\n");
                    printf("%s detach from root\n", node->name);
                    printf("%s root ref count is %d\n",
                           node->name,
                           (euint32)node->root_ref_count);
                    printf("%s number of input links is %d\n",
                           node->name,
                           (euint32)node->input_map.size());
                    printf("%s number of output links is %d\n",
                           node->name,
                           (euint32)node->output_map.size());
                    printf("##Detach end##\n");
                }
			}
		}
	}
	void push_detach_node(mem_btree_node* node) {
		if (node == tail)
			return;
		if (node == head) { head = node->next; }
		if (node->prev)   { node->prev->next = node->next; }
		if (node->next)   { node->next->prev = node->prev; }
		node->next = NULL;
		node->prev = tail;
        if (tail)
            tail->next = node;
		tail = node;
		if (!head)
			head = node;
	}
	void scan_detach_nodes() {
		mem_btree_node* node = tail;
		while (node) {
			if (!node->TrackBack()) {
                vptr ptr = node->begin_addr;
                if (node == head) { head = node->next; }
                if (node == tail) { tail = node->next; }
                if (node->prev)   { node->prev->next = node->next; }
                if (node->next)   { node->next->prev = node->prev; }
                node = node->prev;
                remove(ptr);
            }
            else {
			    node = node->prev;
            }
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
	void create ( const vptr mem, euint size, const char* name );
	void attach ( const vptr section, vptr mem );
	void detach ( const vptr section, vptr mem );
	virtual xhn::static_string GetName() { return COMMAND_SENDER; }
};

class garbage_collector : public MemObject
{
	template <typename> friend class MemHandle;
	friend class mem_btree_node;
public:
    class MemHandleListener
    {
    public:
        virtual void PreAssign() = 0;
        virtual void PreDest() = 0;
    };
    template <typename T>
    class MemHandle : public BannedAllocObject
    {
        friend class garbage_collector;
    private:
        T* m_ptr;
        MemHandleListener* m_list;
        bool m_isTransfer;
        explicit MemHandle(T* ptr)
		: m_ptr(ptr)
        , m_list(NULL)
        , m_isTransfer(true)
        {
        }
        MemHandle(const MemHandle<T>& ptr)
		: m_ptr((T*)ptr.m_ptr)
        , m_list(NULL)
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
        void SetListener(MemHandleListener* list) {
            m_list = list;
        }
        bool IsTransfer() {
            return m_isTransfer;
        }
        explicit MemHandle()
        : m_ptr(NULL)
        , m_list(NULL)
        , m_isTransfer(false)
        {}
        ~MemHandle()
        {
            if (m_list)
                m_list->PreDest();
            if (m_isTransfer) {
                return;
            }
            if (m_ptr) {
                garbage_collector::get()->detach((vptr)this, (vptr)m_ptr);
            }
            m_ptr = NULL;
        }
        const MemHandle& operator = (const MemHandle& ptr) {
            if (m_list)
                m_list->PreAssign();
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
	vptr alloc(euint size, const char* _file, euint32 _line, const char* _name);
	void attach(vptr section, vptr mem);
	void detach(vptr section, vptr mem);
public:
	static garbage_collector* get();
    template <typename T>
    MemHandle<T> alloc(const char* _file, euint32 _line, const char* _name) {
		vptr ptr = alloc(sizeof(T), _file, _line, _name);
		new(ptr) T();
		return MemHandle<T>((T*)ptr);
	}
};
}

#define GC_ALLOC(t, n) xhn::garbage_collector::get()->alloc<t>(__FILE__, __LINE__, n)
#endif