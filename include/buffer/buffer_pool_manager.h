#pragma once
#include "storage/page.h"
#include "disk/disk_manager.h"
#include <cstdint>
#include <unordered_map>
namespace dbengine {
constexpr int POOL_SIZE = 3;
class BufferPoolManager {
    Page pages_[POOL_SIZE];
    uint64_t last_used_[POOL_SIZE]{};
    uint64_t clock_ = 0;
    DiskManager* disk_manager_;
    std::unordered_map<page_id_t, int> page_table_;
    int FindFreeOrEvictFrame() const;
public:
    explicit BufferPoolManager(DiskManager* disk_manager);
    Page* FetchPage(page_id_t page_id);
    bool UnpinPage(page_id_t page_id, bool is_dirty);
    void FlushAllPages();
};
}
