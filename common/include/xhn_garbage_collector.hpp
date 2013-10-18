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
#include "xhn_config.hpp"
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
    scan_mem_node_action()
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
	mem_btree_node* root_head;
	mem_btree_node* root_tail;
    esint32 command_count;
    bool m_isDebugging;
public:
    garbage_collect_robot()
    : orphan(NULL)
    , head(NULL)
    , tail(NULL)
	, root_head(NULL)
	, root_tail(NULL)
    , command_count(0)
#ifdef PRINT_ATTACH_INFO
	, m_isDebugging(true)
#else
    , m_isDebugging(false)
#endif
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
		vptr ret = Malloc(size);
#ifdef PRINT_ALLOC_INFO
    #if BIT_WIDTH == 32 
		printf("alloc %x\n", (ref_ptr)ret);
    #else
		printf("alloc %llx\n", (ref_ptr)ret);
    #endif
#endif
		return ret;
	}
	void insert ( const vptr p, euint s, const char* n, destructor d ) {
		mem_btree_node* node = m_btree.insert((vptr)p, s, n, d);
        push_orphan_node(node);
	}
	void remove ( const vptr p ) {
		if (m_btree.remove((vptr)p)) {
#ifdef PRINT_ALLOC_INFO
    #if BIT_WIDTH == 32 
			printf("free %x\n", (ref_ptr)p);
    #else
			printf("free %llx\n", (ref_ptr)p);
    #endif
#endif
			return Mfree(p);
		}
	}
	void attach ( const vptr section, vptr mem ) {
		mem_btree_node* node = m_btree.find(mem);
		if (node) {
			mem_btree_node* parent = m_btree.find(section);
			if (parent) {
				parent->Attach(section, node);
#ifdef GC_DEBUG
                if (m_isDebugging) {
                    printf("##Attach begin##\n");
					if (node->name && parent->name) {
                        printf("%s attach to the %s\n", node->name, parent->name);
						printf("%s root ref count is %d\n",
							node->name,
							(euint32)node->root_ref_count);
						printf("%s number of output links is %d\n",
							node->name,
							(euint32)node->output_map.size());
						printf("%s root ref count is %d\n",
							parent->name,
							(euint32)parent->root_ref_count);
						printf("%s number of input links is %d\n",
							parent->name,
							(euint32)parent->input_set.size());
						printf("%s number of output links is %d\n",
							parent->name,
							(euint32)parent->output_map.size());
					}
					else {
#if BIT_WIDTH == 32
						printf("%x attach to the %x\n", 
							(ref_ptr)node->begin_addr, 
							(ref_ptr)parent->begin_addr);
						printf("%x root ref count is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->root_ref_count);
						printf("%x number of input links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->input_set.size());
						printf("%x number of output links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->output_map.size());
						printf("%x root ref count is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->root_ref_count);
						printf("%x number of input links is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->input_set.size());
						printf("%x number of output links is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->output_map.size());
#else
						printf("%llx attach to the %llx\n", 
							(ref_ptr)node->begin_addr, 
							(ref_ptr)parent->begin_addr);
						printf("%llx root ref count is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->root_ref_count);
						printf("%llx number of input links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->input_set.size());
						printf("%llx number of output links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->output_map.size());
						printf("%llx root ref count is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->root_ref_count);
						printf("%llx number of input links is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->input_set.size());
						printf("%llx number of output links is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->output_map.size());
#endif
					}
                    printf("##Attach end##\n");
                }
#endif
			}
			else {
				node->AttchToRoot();
#ifdef GC_DEBUG
                if (m_isDebugging) {
					printf("##Attach begin##\n");
					if (node->name) {
						printf("%s attach to root\n", node->name);
						printf("%s root ref count is %d\n",
							node->name,
							(euint32)node->root_ref_count);
						printf("%s number of input links is %d\n",
							node->name,
							(euint32)node->input_set.size());
						printf("%s number of output links is %d\n",
							node->name,
							(euint32)node->output_map.size());
					}
					else {
#if BIT_WIDTH == 32
						printf("%x attach to root\n", 
							(ref_ptr)node->begin_addr);
						printf("%x root ref count is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->root_ref_count);
						printf("%x number of input links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->input_set.size());
						printf("%x number of output links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->output_map.size());
#else
						printf("%llx attach to root\n", 
							(ref_ptr)node->begin_addr);
						printf("%llx root ref count is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->root_ref_count);
						printf("%llx number of input links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->input_set.size());
						printf("%llx number of output links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->output_map.size());
#endif
					}
					printf("##Attach end##\n");
                }
#endif
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
#ifdef GC_DEBUG
                if (m_isDebugging) {
                    printf("##Detach begin##\n");
					if (node->name && parent->name) {
						printf("%s detach from the %s\n", node->name, parent->name);
						printf("%s root ref count is %d\n",
							node->name,
							(euint32)node->root_ref_count);
						printf("%s number of input links is %d\n",
							node->name,
							(euint32)node->input_set.size());
						printf("%s number of output links is %d\n",
							node->name,
							(euint32)node->output_map.size());
						printf("%s root ref count is %d\n",
							parent->name,
							(euint32)parent->root_ref_count);
						printf("%s number of input links is %d\n",
							parent->name,
							(euint32)parent->input_set.size());
						printf("%s number of output links is %d\n",
							parent->name,
							(euint32)parent->output_map.size());
					}
					else {
#if BIT_WIDTH == 32
						printf("%x detach from the %x\n", 
							(ref_ptr)node->begin_addr, 
							(ref_ptr)parent->begin_addr);
						printf("%x root ref count is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->root_ref_count);
						printf("%x number of input links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->input_set.size());
						printf("%x number of output links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->output_map.size());
						printf("%x root ref count is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->root_ref_count);
						printf("%x number of input links is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->input_set.size());
						printf("%x number of output links is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->output_map.size());
#else
						printf("%llx detach from the %x\n", 
							(ref_ptr)node->begin_addr, 
							(ref_ptr)parent->begin_addr);
						printf("%llx root ref count is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->root_ref_count);
						printf("%llx number of input links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->input_set.size());
						printf("%llx number of output links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->output_map.size());
						printf("%llx root ref count is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->root_ref_count);
						printf("%llx number of input links is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->input_set.size());
						printf("%llx number of output links is %d\n",
							(ref_ptr)parent->begin_addr,
							(euint32)parent->output_map.size());
#endif
					}
                    printf("##Detach end##\n");
                }
#endif
			}
			else {
				node->DetachFromRoot();
#ifdef GC_DEBUG
                if (m_isDebugging) {
                    printf("##Detach begin##\n");
					if (node->name) {
						printf("%s detach from root\n", node->name);
						printf("%s root ref count is %d\n",
							node->name,
							(euint32)node->root_ref_count);
						printf("%s number of input links is %d\n",
							node->name,
							(euint32)node->input_set.size());
						printf("%s number of output links is %d\n",
							node->name,
							(euint32)node->output_map.size());
					}
					else {
#if BIT_WIDTH == 32
						printf("%x detach from root\n", (ref_ptr)node->begin_addr);
						printf("%x root ref count is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->root_ref_count);
						printf("%x number of input links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->input_set.size());
						printf("%x number of output links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->output_map.size());
#else
						printf("%llx detach from root\n", (ref_ptr)node->begin_addr);
						printf("%llx root ref count is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->root_ref_count);
						printf("%llx number of input links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->input_set.size());
						printf("%llx number of output links is %d\n",
							(ref_ptr)node->begin_addr,
							(euint32)node->output_map.size());
#endif
					}
                    printf("##Detach end##\n");
                }
#endif
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

		if (node->root_ref_count) {
			if (node == root_tail)
				return;
			if (node == root_head)   { root_head = node->root_next; }
			if (node->root_prev)     { node->root_prev->root_next = node->root_next; }
			if (node->root_next)     { node->root_next->root_prev = node->root_prev; }
			node->root_next = NULL;
			node->root_prev = root_tail;
			if (root_tail)
				root_tail->root_next = node;
			root_tail = node;
			if (!root_head)
				root_head = node;
		}
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
			node->is_garbage = true;
			node = node->prev;
		}
		node = root_tail;
		while (node) {
			if (node->root_ref_count) {
                node->MakeNotGarbage();
			}
			node = node->root_prev;
		}
		node = tail;
		while (node) {
			if (node->is_garbage) {
				vptr ptr = node->begin_addr;
				if (node == head) { head = node->next; }
				if (node == tail) { tail = node->prev; }
				if (node->prev)   { node->prev->next = node->next; }
				if (node->next)   { node->next->prev = node->prev; }

				if (node == root_head) { root_head = node->root_next; }
				if (node == root_tail) { root_tail = node->root_prev; }
				if (node->root_prev)   { node->root_prev->root_next = node->root_next; }
				if (node->root_next)   { node->root_next->root_prev = node->root_prev; }
				node = node->prev;
				remove(ptr);
			}
			else {
				node = node->prev;
			}
		}
	}
	euint get_alloced_mem_size() {
		return MemAllocator_get_alloced_mem_size(m_mem_allocator);
	}
	virtual bool CommandProcImpl(xhn::static_string sender, RobotCommand* command);
	virtual bool CommandReceiptProcImpl(xhn::static_string sender, RobotCommandReceipt* receipt) { return true; }
	virtual xhn::static_string GetName() { return COMMAND_RECEIVER; }
};

class sender_robot : public Robot
{
    DeclareRTTI;
public:
    SafedBuffer* m_channel;
public:
    sender_robot()
    : m_channel(NULL)
    {};
	void create ( const vptr mem, euint size, const char* name, destructor dest );
	void attach ( const vptr section, vptr mem );
	void detach ( const vptr section, vptr mem );
	virtual xhn::static_string GetName() { return COMMAND_SENDER; }
	float get_blockrate();
};
    
/// 如果一个mem_node带有析构函数，且该mem_node下挂载了一个mem_handle
/// 则析构函数调用时机就十分重要
/// 若在从mem_map里删除mem_node时调用析构函数，则有可能在删除mem_node时触发另一次mem_node删除
/// 若该mem_node和父mem_node相同，则会导致一个mem_node被删除两次
/// 若在mem_node删除完毕后调用析构函数，则在查找mem_handle的父mem_node时会找不到，则视为从root上detach
/// 也会出问题
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
    template <typename T>
    class mem_handle : public BannedAllocObject
    {
        friend class garbage_collector;
    private:
        T* m_ptr;
        bool m_is_transfer;
        explicit mem_handle(T* ptr)
		: m_ptr(ptr)
        , m_is_transfer(true)
        {
        }
        mem_handle(const mem_handle<T>& ptr)
		: m_ptr((T*)ptr.m_ptr)
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
        bool is_transfer() {
            return m_is_transfer;
        }
        explicit mem_handle()
        : m_ptr(NULL)
        , m_is_transfer(false)
        {}
        ~mem_handle()
        {
#ifndef GC_DEBUG
			if (is_garbage_collect_robot_thread())
			    return;
#endif
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
			if (ptr.m_ptr) {
                garbage_collector::get()->attach((vptr)this, (vptr)ptr.m_ptr);
			}
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
            return m_ptr != NULL;
        }
    };
	template<typename T>
	class array
	{
		friend class garbage_collector;
	private:
		euint m_element_count;
        T* m_elements;
		bool m_is_transfer;
	private:
		explicit array(T* arr, euint ele_cnt)
			: m_element_count(ele_cnt)
		    , m_elements(arr)
			, m_is_transfer(true)
		{
			for (euint i = 0; i < ele_cnt; i++) {
				new(&arr[i]) T();
			}
		}
		array(const array<T>& arr)
			: m_element_count(arr.m_element_count)
		    , m_elements((T*)arr.m_elements)
			, m_is_transfer(false)
		{
			if (m_elements == arr.m_elements)
				return;
			if (m_elements) {
				garbage_collector::get()->detach((vptr)this, (vptr)m_elements);
			}
			m_elements = arr.m_elements;
			garbage_collector::get()->attach((vptr)this, (vptr)arr.m_elements);
		}
	public:
		bool is_transfer() {
			return m_is_transfer;
		}
		explicit array()
			: m_elements(NULL)
			, m_is_transfer(false)
		{}
		~array()
		{
#ifndef GC_DEBUG
			if (is_garbage_collect_robot_thread())
				return;
#endif
			if (m_is_transfer) {
				return;
			}
			if (m_elements) {
				garbage_collector::get()->detach((vptr)this, (vptr)m_elements);
			}
			m_elements = NULL;
		}
		const array& operator = (const array& ptr) {
			if (m_elements == ptr.m_elements)
				return *this;
			if (m_elements) {
				garbage_collector::get()->detach((vptr)this, (vptr)m_elements);
			}
			m_elements = ptr.m_elements;
			m_element_count = ptr.m_element_count;
			if (ptr.m_elements) {
				garbage_collector::get()->attach((vptr)this, (vptr)ptr.m_elements);
			}
			return *this;
		}
		T& operator[] (euint index) {
			EAssert(m_elements, "array is NULL");
			EAssert(index < m_element_count, "array index out of bounds");
			return m_elements[index];
		}
		operator bool () const {
			return m_elements != NULL;
		}
		euint size() const {
            return m_element_count;
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
	template <typename T>
	array<T> alloc_array(euint size, const char* _file, euint32 _line, const char* _name) {
		vptr ptr = alloc(sizeof(T) * size, _file, _line, _name, NULL);
		return array<T>((T*)ptr, size);
	}
	float get_blockrate() {
		return m_sender->get_blockrate();
	}
};
}

#define GC_ALLOC(t) xhn::garbage_collector::get()->alloc<t>(__FILE__, __LINE__, NULL)
#define GC_ALLOC_ARRAY(t, s) xhn::garbage_collector::get()->alloc_array<t>(s, __FILE__, __LINE__, NULL)
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