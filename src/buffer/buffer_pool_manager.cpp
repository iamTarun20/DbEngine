// src/buffer/buffer_pool_manager.cpp
#include "buffer/buffer_pool_manager.h" // .h use kiya hai

namespace dbengine {

BufferPoolManager::BufferPoolManager(DiskManager* disk_manager)
    : disk_manager_(disk_manager) {}

int BufferPoolManager::FindFreeOrEvictFrame() {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (pages_[i].page_id_ == -1) return i;
    }

    for (int i = 0; i < POOL_SIZE; i++) {
        if (pages_[i].pin_count_ == 0) {
            page_table_.erase(pages_[i].page_id_);
            return i;
        }
    }
    return -1;
}

Page* BufferPoolManager::FetchPage(page_id_t page_id) {
    if (page_table_.find(page_id) != page_table_.end()) {
        int frame_id = page_table_[page_id];
        pages_[frame_id].pin_count_++;
        std::cout << "[BPM] Cache Hit! Page " << page_id << " RAM me mil gaya.\n";
        return &pages_[frame_id];
    }

    int target_frame = FindFreeOrEvictFrame();
    if (target_frame == -1) {
        std::cout << "[BPM] Error: Sabhi pages pinned hain!\n";
        return nullptr;
    }

    if (pages_[target_frame].is_dirty_) {
        std::cout << "[BPM] Saving Dirty Page " << pages_[target_frame].page_id_ << " to Disk...\n";
        disk_manager_->WritePage(pages_[target_frame].page_id_, pages_[target_frame].data_.GetRawData());
    }

    std::cout << "[BPM] Fetching Page " << page_id << " from Disk...\n";
    pages_[target_frame].ResetMemory();

    disk_manager_->ReadPage(page_id, const_cast<char*>(pages_[target_frame].data_.GetRawData()));

    pages_[target_frame].page_id_ = page_id;
    pages_[target_frame].pin_count_ = 1;
    pages_[target_frame].is_dirty_ = false;

    page_table_[page_id] = target_frame;

    return &pages_[target_frame];
}

void BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty) {
    if (page_table_.find(page_id) != page_table_.end()) {
        int frame_id = page_table_[page_id];

        if (is_dirty) {
            pages_[frame_id].is_dirty_ = true;
        }

        if (pages_[frame_id].pin_count_ > 0) {
            pages_[frame_id].pin_count_--;
        }
    }
}

} // namespace dbengine