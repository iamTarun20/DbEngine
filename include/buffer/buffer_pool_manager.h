#pragma once
#include "storage/page.h"
#include "disk/disk_manager.h"
#include <cstdint>
#include <unordered_map>
namespace dbengine {
constexpr int POOL_SIZE = 3;

struct BufferPoolStats {
    uint64_t cache_hits = 0;
    uint64_t cache_misses = 0;
    uint64_t evictions = 0;
    uint64_t page_reads = 0;
    uint64_t page_writes = 0;
};

class BufferPoolManager {
    Page pages_[POOL_SIZE];
    uint64_t last_used_[POOL_SIZE]{};
    uint64_t clock_ = 0;
    DiskManager* disk_manager_;
    std::unordered_map<page_id_t, int> page_table_;
    BufferPoolStats stats_;
    int FindFreeOrEvictFrame() const;
    void WritePage(page_id_t page_id, const SlottedPage& data);
public:
    explicit BufferPoolManager(DiskManager* disk_manager);
    page_id_t GetPageCount() const { return disk_manager_->GetPageCount(); }
    BufferPoolStats GetStats() const { return stats_; }
    Page* FetchPage(page_id_t page_id);
    // Append an initialized page and leave it in the cache without a pin.
    bool AppendPage(const SlottedPage& data);
    bool UnpinPage(page_id_t page_id, bool is_dirty);
    void FlushAllPages();
};
}
