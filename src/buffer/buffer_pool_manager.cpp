#include "buffer/buffer_pool_manager.h"
#include <stdexcept>
namespace dbengine {
BufferPoolManager::BufferPoolManager(DiskManager* disk_manager) : disk_manager_(disk_manager) {
    if (!disk_manager_) throw std::invalid_argument("Disk manager is required");
}
int BufferPoolManager::FindFreeOrEvictFrame() const {
    int victim = -1;
    for (int i = 0; i < POOL_SIZE; ++i) {
        if (pages_[i].page_id_ == -1) return i;
        if (pages_[i].pin_count_ == 0 && (victim == -1 || last_used_[i] < last_used_[victim])) victim = i;
    }
    return victim;
}
Page* BufferPoolManager::FetchPage(page_id_t page_id) {
    if (page_id < 0) throw std::invalid_argument("Invalid page ID");
    auto found = page_table_.find(page_id);
    if (found != page_table_.end()) {
        ++stats_.cache_hits;
        Page& page = pages_[found->second];
        ++page.pin_count_;
        last_used_[found->second] = ++clock_;
        return &page;
    }
    ++stats_.cache_misses;
    const int frame = FindFreeOrEvictFrame();
    if (frame == -1) return nullptr;
    Page& page = pages_[frame];
    // Read first so an I/O or validation error cannot discard the old frame.
    SlottedPage incoming;
    if (disk_manager_->ReadPage(page_id, incoming.GetRawData())) {
        ++stats_.page_reads;
        if (!incoming.IsValid()) throw std::runtime_error("Invalid or corrupted database page");
    }
    if (page.is_dirty_) WritePage(page.page_id_, page.data_);
    page_table_.emplace(page_id, frame);
    if (page.page_id_ != -1) {
        page_table_.erase(page.page_id_);
        ++stats_.evictions;
    }
    page.data_ = incoming;
    page.page_id_ = page_id;
    page.pin_count_ = 1;
    page.is_dirty_ = false;
    last_used_[frame] = ++clock_;
    return &page;
}
bool BufferPoolManager::AppendPage(const SlottedPage& data) {
    if (!data.IsValid()) throw std::invalid_argument("Invalid page contents");
    const page_id_t page_id = GetPageCount();
    Page* page = FetchPage(page_id);
    if (!page) return false;
    if (page->pin_count_ != 1 || page->is_dirty_) {
        UnpinPage(page_id, false);
        return false;
    }
    try {
        // Persist a complete page before its ID becomes visible to a scan.
        WritePage(page_id, data);
    } catch (...) {
        UnpinPage(page_id, false);
        throw;
    }
    page->data_ = data;
    UnpinPage(page_id, false);
    return true;
}
void BufferPoolManager::WritePage(page_id_t page_id, const SlottedPage& data) {
    disk_manager_->WritePage(page_id, data.GetRawData());
    ++stats_.page_writes;
}
bool BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty) {
    auto found = page_table_.find(page_id);
    if (found == page_table_.end() || pages_[found->second].pin_count_ == 0) return false;
    Page& page = pages_[found->second];
    page.is_dirty_ = page.is_dirty_ || is_dirty;
    --page.pin_count_;
    last_used_[found->second] = ++clock_;
    return true;
}
void BufferPoolManager::FlushAllPages() {
    for (Page& page : pages_) {
        if (page.page_id_ != -1 && page.is_dirty_) {
            WritePage(page.page_id_, page.data_);
            page.is_dirty_ = false;
        }
    }
}
}
