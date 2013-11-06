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

void GetPaths(const char* baseFolder,
              xhn::vector<xhn::string>& subFolders,
              xhn::vector<xhn::string>& paths)
{
    NSFileManager *fileManager = [[NSFileManager alloc] init];
    NSString *strFolder = [NSString stringWithCString:baseFolder encoding:NSUTF8StringEncoding];
    NSURL *directoryURL = [[NSURL alloc] initWithString:strFolder]; // URL pointing to the directory you want to browse
    NSArray *keys = [NSArray arrayWithObject:NSURLIsDirectoryKey];
    
    NSDirectoryEnumerator *enumerator = [fileManager
                                         enumeratorAtURL:directoryURL
                                         includingPropertiesForKeys:keys
                                         options:0
                                         errorHandler:^(NSURL *url, NSError *error) {
                                             // Handle the error.
                                             // Return YES if the enumeration should continue after the error.
                                             return YES;
                                         }];
    
    for (NSURL *url in enumerator) {
        NSError *error;
        NSNumber *isDirectory = nil;
        if (! [url getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:&error]) {
            // handle error
        }
        else if (! [isDirectory boolValue]) {
            // No error and it’s not a directory; do something with the file
            NSString* path = [url path];
            xhn::string strPath = [path UTF8String];
            paths.push_back(strPath);
        }
        else if ( [isDirectory boolValue]) {
            //
            NSString* subFolder = [url path];
            xhn::string strSubFolder = [subFolder UTF8String];
            subFolders.push_back(strSubFolder);
        }
    }
}

AppleFileManager::AppleFileManager()
{
    m_fileManager = (__bridge vptr)[[NSFileManager alloc] init];
}
AppleFileManager::~AppleFileManager()
{
    NSFileManager *fileManager = (__bridge NSFileManager *)m_fileManager;
    [fileManager release];
}
bool AppleFileManager::is_exist(const xhn::wstring& dir, bool& is_directory)
{
    NSFileManager *fileManager = (__bridge NSFileManager *)m_fileManager;
    xhn::Utf8 utf8(dir.c_str());
    NSString *strDir = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
    BOOL isDir = NO;
    BOOL ret = [fileManager fileExistsAtPath:strDir isDirectory:&isDir];
    [strDir release];
    is_directory = (isDir == YES);
    return ret == YES;
}
bool AppleFileManager::create_directory(const xhn::wstring& dir)
{
    NSFileManager *fileManager = (NSFileManager *)m_fileManager;
    xhn::Utf8 utf8(dir.c_str());
    NSString *strDir = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
    BOOL ret = [fileManager createDirectoryAtPath:strDir
                            withIntermediateDirectories:NO
                            attributes:nil
                            error:nil];
    [strDir release];
    return ret == YES;
}
bool AppleFileManager::create_file(const xhn::wstring& path)
{
    NSString *str = @"";
    xhn::Utf8 utf8(path.c_str());
    NSString *strPath = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
    BOOL ret = [str writeToFile:strPath
                    atomically:YES
                    encoding:NSUTF8StringEncoding
                    error:nil];
    [str release];
    [strPath release];
    return ret = YES;
}
xhn::file_stream_ptr AppleFileManager::open(const xhn::wstring& path)
{
    xhn::file_stream_ptr ret;
    {
        NSFileManager *fileManager = (NSFileManager *)m_fileManager;
        xhn::Utf8 utf8(path.c_str());
        NSString *strPath = [NSString stringWithCString:((xhn::string)utf8).c_str() encoding:NSUTF8StringEncoding];
        BOOL isDir = NO;
        BOOL isExist = [fileManager fileExistsAtPath:strPath isDirectory:&isDir];
        if (isExist == YES && isDir == NO) {
            AppleFile* file = ENEW AppleFile;
            file->m_path = path;
            NSFileHandle *fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:strPath];
            file->m_fileHandle = fileHandle;
            ret = file;
        }
        [strPath release];
    }
    return ret;
}
xhn::file_stream_ptr AppleFileManager::create_and_open(const xhn::wstring& path)
{
    create_file(path);
    return open(path);
}
AppleFile::~AppleFile()
{
    if (m_fileHandle) {
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
        [fileHandle closeFile];
        [fileHandle release];
    }
}

euint64 AppleFile::read(euint8* buffer, euint64 size)
{
    euint64 ret = 0;
    if (m_fileHandle) {
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
        NSData* data = [fileHandle readDataOfLength: size];
        ret = [data length];
        [data getBytes:buffer length:size];
        [data release];
    }
    return ret;
}
bool AppleFile::write(const euint8* buffer, euint64 size)
{
    if (m_fileHandle) {
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
        NSData* data = [NSData dataWithBytes:buffer length:size];
        [fileHandle writeData:data];
        [data release];
        return true;
    }
    else
        return false;
}
euint64 AppleFile::get_pos()
{
    euint64 ret = 0;
    if (m_fileHandle) {
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
        ret = [fileHandle offsetInFile];
    }
    return ret;
}
euint64 AppleFile::set_pos(euint64 pos)
{
    euint64 ret = 0;
    if (m_fileHandle) {
        NSFileHandle *fileHandle = (NSFileHandle*)m_fileHandle;
        [fileHandle seekToFileOffset:pos];
        ret = [fileHandle offsetInFile];
    }
    return ret;
}
/**
euint8& AppleFile::operator[] (euint64 pos)
{
    ///
}
 **/
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