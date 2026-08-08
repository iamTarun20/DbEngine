// include/buffer/buffer_pool_manager.h
#pragma once
#include "storage/page.h"         // .h use kiya hai
#include "disk/disk_manager.h"    // .h use kiya hai
#include <unordered_map>
#include <iostream>

namespace dbengine {

    constexpr int POOL_SIZE = 3;

    class BufferPoolManager {
    private:
        Page pages_[POOL_SIZE];
        DiskManager* disk_manager_;
        std::unordered_map<page_id_t, int> page_table_;

        int FindFreeOrEvictFrame();

    public:
        BufferPoolManager(DiskManager* disk_manager);

        Page* FetchPage(page_id_t page_id);
        void UnpinPage(page_id_t page_id, bool is_dirty);
    };

} // namespace dbengine