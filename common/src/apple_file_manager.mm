/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

//
//  apple_file_manager.mm
//  ecg
//
//  Created by 徐 海宁 on 13-4-25.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#include "apple_file_manager.h"
#import <Foundation/Foundation.h>

void GetFilenames(FileDirectory dir, FilenameArray* filenames)
{
    NSFileManager *defaultManager;
    defaultManager = [NSFileManager defaultManager];
    NSArray *documentPaths = NULL;
    switch (dir)
    {
        case AppDirectory:
            documentPaths = NSSearchPathForDirectoriesInDomains(NSApplicationDirectory, NSUserDomainMask, YES);
            break;
        case UsrDirectory:
            documentPaths = NSSearchPathForDirectoriesInDomains(NSUserDirectory, NSUserDomainMask, YES);
            break;
        default:
            documentPaths = NSSearchPathForDirectoriesInDomains(NSApplicationDirectory, NSUserDomainMask, YES);
            break;
    }
    for(NSString *filename in documentPaths)
    {
        const char *str = [filename cStringUsingEncoding:NSASCIIStringEncoding];
        filenames->AddFilename(str);
    }
}

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