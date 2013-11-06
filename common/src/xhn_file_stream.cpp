//
//  xhn_file_stream.cpp
//  7z
//
//  Created by 徐 海宁 on 13-11-5.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#include "xhn_file_stream.hpp"
#if defined(__APPLE__)
#include "apple_file_manager.h"
static AppleFileManager* s_AppleFileManager = NULL;
xhn::file_manager* xhn::file_manager::get()
{
    if (!s_AppleFileManager) {
        s_AppleFileManager = ENEW AppleFileManager;
    }
    return s_AppleFileManager;
}
#endif