//
//  xhn_file_stream.h
//  7z
//
//  Created by 徐 海宁 on 13-11-5.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#ifndef ___z__xhn_file_stream__
#define ___z__xhn_file_stream__

#include "common.h"
#include "etypes.h"

#include "emem.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_string.hpp"

#include "xhn_btree.hpp"
#include "xhn_void_vector.hpp"

#include "xhn_lock.hpp"

#define BLOCK_SIZE 4096
#define MAX_BLOCKS 16
namespace xhn
{
    class file_stream;
    class FFileBlockAllocator;
    class file_block : public btree_node<euint64, euint64>
    {
    public:
        file_block* prev;
        file_block* next;
        euint64 buffered_size;
        file_stream* stream;
        btree<file_block, euint64, euint64, FFileBlockAllocator>* tree;
        void_vector buffer;
    public:
        file_block();
        ~file_block();
        void write(euint64 offs, const euint8* buf, euint64 size);
        void read(euint64 offs, euint8* buf, euint64 size);
        void init();
        euint8* access(euint64 offs, euint64 size);
        const euint8* access(euint64 offs, euint64 size) const;
    };
    class FFileBlockAllocator
    {
    public:
        FFileBlockAllocator() {}
        void deallocate(file_block* ptr, euint) { Mfree(ptr); }
        file_block* allocate(euint count) { return (file_block*)Malloc(count * sizeof(file_block)); }
        void construct(file_block* ptr, vptr) { new ( ptr ) file_block (); }
        void pre_destroy(file_block* ptr) {}
        void destroy(file_block* ptr) { ptr->~file_block(); }
    };
    
    class file_stream : public RefObject
    {
        friend class file_block;
    private:
        MutexLock lock;
        file_block* head;
        file_block* tail;
        euint64 file_block_count;
        file_block* cache0;
        file_block* cache1;
        btree<
        file_block,
        euint64,
        euint64,
        xhn::FFileBlockAllocator> file_block_tree;
        SpinLock spin_lock;
    protected:
        static xhn::file_block* access(file_stream* file, euint64 pos);
        void synchronize_file();
        void push_file_block(xhn::file_block* node);
    public:
        file_stream()
        : head(NULL)
        , tail(NULL)
        , file_block_count(0)
        , cache0(NULL)
        , cache1(NULL)
        {}
        virtual ~file_stream() {}
        virtual euint64 read(euint8* buffer, euint64 size) = 0;
        virtual bool write(const euint8* buffer, euint64 size) = 0;
        virtual euint64 get_size() = 0;
        virtual euint64 get_pos() = 0;
        virtual euint64 set_pos(euint64 pos) = 0;
        virtual void set_base_offset(euint64 offs) = 0;
        virtual euint8& operator[] (euint64 pos) = 0;
        virtual const euint8& operator[] (euint64 pos)const = 0;
    };
    typedef xhn::SmartPtr<file_stream> file_stream_ptr;
    class file_manager : public MemObject
    {
    public:
        virtual ~file_manager() {}
        virtual bool is_exist(const xhn::wstring& path, bool& is_directory) = 0;
        virtual bool create_directory(const xhn::wstring& dir) = 0;
        virtual bool create_file(const xhn::wstring& path) = 0;
        virtual file_stream_ptr open(const xhn::wstring& path) = 0;
        virtual file_stream_ptr create_and_open(const xhn::wstring& path) = 0;
        static file_manager* get();
    };
}

#endif /* defined(___z__xhn_file_stream__) */
