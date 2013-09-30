//
//  apple_file_manager.cpp
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