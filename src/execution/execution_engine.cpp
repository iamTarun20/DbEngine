#include "execution/execution_engine.h"
#include <iostream>
#include <stdexcept>
namespace dbengine {
namespace {
class PagePin {
    BufferPoolManager& pool_;
    page_id_t page_id_;
    Page* page_;
    bool dirty_ = false;
public:
    PagePin(BufferPoolManager& pool, page_id_t page_id)
        : pool_(pool), page_id_(page_id), page_(pool.FetchPage(page_id)) {}
    ~PagePin() { if (page_) pool_.UnpinPage(page_id_, dirty_); }
    PagePin(const PagePin&) = delete;
    PagePin& operator=(const PagePin&) = delete;

    explicit operator bool() const { return page_ != nullptr; }
    SlottedPage& Data() const { return page_->data_; }
    void MarkDirty() { dirty_ = true; }
};
}

ExecutionEngine::ExecutionEngine(BufferPoolManager* buffer_pool) : buffer_pool_(buffer_pool) {
    if (!buffer_pool_) throw std::invalid_argument("Buffer pool is required");
}
void ExecutionEngine::Execute(const SQLStatement& statement) {
    if (statement.type == StatementType::INVALID || statement.table_name.empty()) {
        std::cout << "Invalid statement.\n";
        return;
    }
    const std::string prefix = statement.table_name + "\t";
    if (statement.type == StatementType::INSERT) {
        Insert(prefix + statement.record_data);
    } else {
        Select(prefix);
    }
}

void ExecutionEngine::Insert(const std::string& record) {
    const page_id_t page_count = buffer_pool_->GetPageCount();
    if (page_count > 0) {
        PagePin tail(*buffer_pool_, page_count - 1);
        if (!tail) {
            std::cout << "No buffer frame available.\n";
            return;
        }
        if (tail.Data().InsertRecord(record)) {
            tail.MarkDirty();
            std::cout << "1 row inserted.\n";
            return;
        }
    }

    SlottedPage new_page;
    if (!new_page.InsertRecord(record)) {
        std::cout << "Row exceeds the capacity of one 4 KiB page.\n";
        return;
    }
    std::cout << (buffer_pool_->AppendPage(new_page)
        ? "1 row inserted.\n" : "No buffer frame available.\n");
}

void ExecutionEngine::Select(const std::string& prefix) {
    uint64_t count = 0;
    const page_id_t page_count = buffer_pool_->GetPageCount();
    for (page_id_t page_id = 0; page_id < page_count; ++page_id) {
        PagePin page(*buffer_pool_, page_id);
        if (!page) {
            std::cout << "No buffer frame available.\n";
            return;
        }
        const SlottedPage& data = page.Data();
        for (uint16_t slot = 0; slot < data.GetRecordCount(); ++slot) {
            const std::string record = data.GetRecord(slot);
            if (record.compare(0, prefix.size(), prefix) == 0) {
                std::cout << record.substr(prefix.size()) << '\n';
                ++count;
            }
        }
    }
    std::cout << count << " row(s) returned.\n";
}
}
