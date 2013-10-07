/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

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
    destructor dest;
public:
    mem_create_command(vptr m, euint s, const char* n, destructor d)
    : mem(m)
    , size(s)
    , name(n)
    , dest(d)
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
    
class scan_orphan_node_action : public Action
{
    DeclareRTTI;
public:
    virtual void DoImpl();
};

class scan_mem_node_action : public Action
{
	DeclareRTTI;
public:
    euint32 m_doCount;
public:
    scan_mem_node_action()
    : m_doCount(0)
    {}
	virtual void DoImpl();
};

class garbage_collect_robot : public Robot
{
	DeclareRTTI;
public:
	static garbage_collect_robot* s_garbage_collect_robot;
public:
    MemAllocator m_mem_allocator;
	mem_btree m_btree;
    mem_btree_node* orphan;
	mem_btree_node* head;
	mem_btree_node* tail;
    esint32 command_count;
    bool m_isDebugging;
public:
    garbage_collect_robot()
    : orphan(NULL)
    , head(NULL)
    , tail(NULL)
    , command_count(0)
    , m_isDebugging(false)
	{
		s_garbage_collect_robot = this;
        m_mem_allocator = MemAllocator_new();
	}
	~garbage_collect_robot() 
	{
        MemAllocator_delete(m_mem_allocator);
	}
	static garbage_collect_robot* get() {
		return s_garbage_collect_robot;
	}
    vptr alloc ( euint size ) {
        return MemAllocator_alloc(m_mem_allocator, size, false);
    }
	void insert ( const vptr p, euint s, const char* n, destructor d ) {
		mem_btree_node* node = m_btree.insert((vptr)p, s, n, d);
        push_orphan_node(node);
	}
	void remove ( const vptr p ) {
		if (m_btree.remove((vptr)p)) {
            MemAllocator_free(m_mem_allocator, p);
		}
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
    void push_orphan_node(mem_btree_node* node) {
        if (orphan) {
            orphan->prev = node;
        }
        node->next = orphan;
        node->prev = NULL;
        orphan = node;
    }
	void push_detach_node(mem_btree_node* node) {
		if (node == tail)
			return;
        if (node == orphan) { orphan = node->next; }
		if (node == head)   { head = node->next; }
		if (node->prev)     { node->prev->next = node->next; }
		if (node->next)     { node->next->prev = node->prev; }
		node->next = NULL;
		node->prev = tail;
        if (tail)
            tail->next = node;
		tail = node;
		if (!head)
			head = node;
	}
    void scan_orphan_nodes(esint32 num_cmds) {
        mem_btree_node* node = orphan;
        while (node) {
            node->orphan_count -= num_cmds;
            if (node->orphan_count <= 0) {
                mem_btree_node* tmp = node->next;
                push_detach_node(node);
                node = tmp;
            }
            else
                node = node->next;
        }
    }
	void scan_detach_nodes() {
		mem_btree_node* node = tail;
		while (node) {
			if (!node->TrackBack()) {
                vptr ptr = node->begin_addr;
                if (node == head) { head = node->next; }
                if (node == tail) { tail = node->prev; }
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
	void create ( const vptr mem, euint size, const char* name, destructor dest );
	void attach ( const vptr section, vptr mem );
	void detach ( const vptr section, vptr mem );
	virtual xhn::static_string GetName() { return COMMAND_SENDER; }
};
    
template <class T>
void gc_destructor(T* ptr)
{
    ptr->T::~T();
};

class garbage_collector : public MemObject
{
	template <typename> friend class mem_handle;
	friend class mem_btree_node;
public:
    class mem_handle_listener
    {
    public:
        virtual void pre_assign() = 0;
        virtual void pre_dest() = 0;
    };
    template <typename T>
    class mem_handle : public BannedAllocObject
    {
        friend class garbage_collector;
    private:
        T* m_ptr;
        mem_handle_listener* m_list;
        bool m_is_transfer;
        explicit mem_handle(T* ptr)
		: m_ptr(ptr)
        , m_list(NULL)
        , m_is_transfer(true)
        {
        }
        mem_handle(const mem_handle<T>& ptr)
		: m_ptr((T*)ptr.m_ptr)
        , m_list(NULL)
        , m_is_transfer(false)
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
        void set_listener(mem_handle_listener* list) {
            m_list = list;
        }
        bool is_transfer() {
            return m_is_transfer;
        }
        explicit mem_handle()
        : m_ptr(NULL)
        , m_list(NULL)
        , m_is_transfer(false)
        {}
        ~mem_handle()
        {
            if (is_garbage_collect_robot_thread())
                return;
            if (m_is_transfer) {
                return;
            }
            if (m_ptr) {
                garbage_collector::get()->detach((vptr)this, (vptr)m_ptr);
            }
            m_ptr = NULL;
        }
        const mem_handle& operator = (const mem_handle& ptr) {
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
            EAssert(m_ptr, "memory handle is NULL");
            return m_ptr;
        }
        T* get() {
            EAssert(m_ptr, "memory handle is NULL");
            return m_ptr;
        }
        T& operator *() {
            EAssert(m_ptr, "memory handle is NULL");
            return *m_ptr;
        }
        operator bool () const {
            return (bool)m_ptr;
        }
    };
private:
	static garbage_collector* s_garbage_collector;
    static RobotThread* s_garbage_collect_robot_thread;
private:
	SpinLock m_senderLock;
	sender_robot* m_sender;
private:
    static void* garbage_collector_proc(void* gc);
private:
	garbage_collector();
	~garbage_collector();
	vptr alloc(euint size,
               const char* _file,
               euint32 _line,
               const char* _name,
               destructor dest);
	void attach(vptr section, vptr mem);
	void detach(vptr section, vptr mem);
public:
	static garbage_collector* get();
    static bool is_garbage_collect_robot_thread();
    template <typename T>
    mem_handle<T> alloc(const char* _file, euint32 _line, const char* _name) {
		vptr ptr = alloc(sizeof(T), _file, _line, _name, (destructor)&gc_destructor<T>);
		new(ptr) T();
		return mem_handle<T>((T*)ptr);
	}
};
}

#define GC_ALLOC(t) xhn::garbage_collector::get()->alloc<t>(__FILE__, __LINE__, NULL)
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