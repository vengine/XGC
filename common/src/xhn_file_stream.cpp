//
//  xhn_file_stream.cpp
//  7z
//
//  Created by 徐 海宁 on 13-11-5.
//  Copyright (c) 2013年 徐 海宁. All rights reserved.
//

#include "xhn_file_stream.hpp"
#include "xhn_set.hpp"

xhn::file_block::file_block()
: prev(NULL)
, next(NULL)
, buffered_size(0)
, stream(NULL)
{
    buffer.convert<xhn::void_vector::FCharProc>(1);
    buffer.resize(BLOCK_SIZE);
}
xhn::file_block::~file_block()
{
    if (stream) {
        MutexLock::Instance inst = stream->lock.Lock();
        euint64 size = stream->get_size();
        if (begin_addr >= size) {
            euint64 fill_size = begin_addr - size;
            if (fill_size > 0) {
                stream->set_pos(size);
                euint64 num_blocks = fill_size / BLOCK_SIZE;
                for (euint64 i = 0; i < num_blocks; i++) {
                    stream->write((const euint8*)buffer.get(), BLOCK_SIZE);
                }
                if (fill_size % BLOCK_SIZE) {
                    stream->write((const euint8*)buffer.get(), fill_size % BLOCK_SIZE);
                }
            }
        }
        stream->set_pos(begin_addr);
        stream->write((const euint8*)buffer.get(), buffered_size);
    }
}
void xhn::file_block::write(euint64 offs, const euint8* buf, euint64 size)
{
    memcpy(buffer[offs], buf, size);
    if (offs + size > buffered_size)
        buffered_size = offs + size;
}

/// 这里不应该读越界
/// |---------stream size-----|-------------------------|
/// |---------buffered size---|
/// |---------read size-------|--incremented size--|
void xhn::file_block::read(euint64 offs, euint8* buf, euint64 size)
{
    if (offs + size > buffered_size) {
        if (stream) {
            MutexLock::Instance inst = stream->lock.Lock();
            euint64 stream_size = stream->get_size();
            if (stream_size > begin_addr + offs) {
                stream->set_pos(begin_addr + offs);
                euint64 incremented_size = offs + size - buffered_size;
                stream->read((euint8*)buffer[buffered_size], incremented_size);
            }
        }
        buffered_size = offs + size;
    }
    memcpy(buf, buffer[offs], size);
}

void xhn::file_block::init()
{
    MutexLock::Instance inst = stream->lock.Lock();
    euint64 stream_size = stream->get_size();
    if (stream_size > begin_addr) {
        buffered_size = stream_size - begin_addr;
        if (buffered_size > BLOCK_SIZE)
            buffered_size = BLOCK_SIZE;
        stream->set_pos(begin_addr);
        stream->read((euint8*)buffer.get(), buffered_size);
    }
}

euint8* xhn::file_block::access(euint64 offs, euint64 size)
{
    if (offs + size > buffered_size) {
        buffered_size = offs + size;
    }
    return (euint8*)buffer[offs];
}

const euint8* xhn::file_block::access(euint64 offs, euint64 size) const
{
    return (const euint8*)buffer[offs];
}

void xhn::file_stream::synchronize_file()
{
    set<euint64> blocks;
    file_block* node = head;
    while (node) {
        blocks.insert(node->begin_addr);
        node = node->next;
    }
    set<euint64>::iterator iter = blocks.begin();
    set<euint64>::iterator end = blocks.end();
    for (; iter != end; iter++) {
        file_block_tree.remove(*iter);
    }
}

xhn::file_block* xhn::file_stream::access(file_stream* file, euint64 pos)
{
    if (file->cache0 && pos >= file->cache0->begin_addr && pos <= file->cache0->end_addr) {
        return file->cache0;
    }
    else if (file->cache1 && pos >= file->cache1->begin_addr && pos <= file->cache1->end_addr) {
        file_block* tmp = file->cache0;
        file->cache0 = file->cache1;
        file->cache1 = tmp;
        return file->cache0;
    }
    file_block* block = NULL;
    euint64 beginAddr = pos / BLOCK_SIZE * BLOCK_SIZE;
    {
        SpinLock::Instance inst = file->spin_lock.Lock();
        block = file->file_block_tree.find(pos);
        if (!block) {
            if (file->file_block_count == MAX_BLOCKS) {
                file_block* node = file->tail;
                if (node) {
                    if (node == file->head) { file->head = node->next; }
                    if (node == file->tail) { file->tail = node->prev; }
                    if (node->prev)   { node->prev->next = node->next; }
                    if (node->next)   { node->next->prev = node->prev; }
                    file->file_block_tree.remove(node->begin_addr);
                    file->file_block_count--;
                }
                else {
                    VEngineExce(UndesiredBranch, "");
                }
            }
            block = file->file_block_tree.insert(beginAddr, BLOCK_SIZE, NULL);
            block->stream = file;
            block->tree = &file->file_block_tree;
            block->init();
            file->push_file_block(block);
            file->file_block_count++;
        }
        else {
            file->push_file_block(block);
        }
    }
    if (block != file->cache0) {
        file_block* tmp = file->cache0;
        file->cache0 = block;
        file->cache1 = tmp;
    }
    else {
        VEngineExce(UndesiredBranch, "");
    }
    return block;
}

void xhn::file_stream::push_file_block(file_block* node)
{
    if (node != head) {
        if (node == tail)   { tail = node->prev; }
        if (node->prev)     { node->prev->next = node->next; }
        if (node->next)     { node->next->prev = node->prev; }
        node->next = head;
        node->prev = NULL;
        if (head)
            head->prev = node;
        head = node;
        if (!tail)
            tail = node;
    }
}

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