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
namespace xhn
{
    class file_stream : public RefObject
    {
    public:
        virtual ~file_stream() {}
        virtual euint64 read(euint8* buffer, euint64 size) = 0;
        virtual bool write(const euint8* buffer, euint64 size) = 0;
        virtual euint64 get_pos() = 0;
        virtual euint64 set_pos(euint64 pos) = 0;
        ///virtual euint8& operator[] (euint64 pos) = 0;
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
