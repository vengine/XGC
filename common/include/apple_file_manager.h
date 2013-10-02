//
//  apple_file_manager.h
//  ecg
//
//  Created by 徐 海宁 on 13-4-25.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#ifndef __ecg__apple_file_manager__
#define __ecg__apple_file_manager__

class FilenameArray
{
public:
    virtual void AddFilename(const char* filename) = 0;
};

enum FileDirectory
{
    AppDirectory,
    UsrDirectory,
};

void GetFilenames(FileDirectory dir, FilenameArray* filenames);
#endif /* defined(__ecg__apple_file_manager__) */
