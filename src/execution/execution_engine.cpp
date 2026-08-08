// src/execution/execution_engine.cpp
#include "execution/execution_engine.h"

namespace dbengine {

    ExecutionEngine::ExecutionEngine(BufferPoolManager* bpm) : bpm_(bpm) {}

    void ExecutionEngine::Execute(const SQLStatement& stmt) {
        // 1. SELECT LOGIC
        if (stmt.type == StatementType::SELECT) {
            // Buffer Pool se table ka page mangwao
            Page* page = bpm_->FetchPage(0);
            if (page != nullptr) {
                int count = page->data_.GetRecordCount();
                std::cout << "--> [Executor] Fetching data from table '" << stmt.table_name << "'\n";
                std::cout << "----------------------------------------\n";

                for (int i = 0; i < count; i++) {
                    std::cout << " Row " << i << ": " << page->data_.GetRecord(i) << "\n";
                }

                std::cout << "----------------------------------------\n";
                std::cout << "--> [Success] " << count << " rows returned.\n\n";

                // Padhne ke baad page wapas rakh do (Dirty = false)
                bpm_->UnpinPage(0, false);
            } else {
                std::cout << "--> [Error] Failed to fetch page from disk.\n\n";
            }
        }
        // 2. INSERT LOGIC
        else if (stmt.type == StatementType::INSERT) {
            // Buffer Pool se page mangwao
            Page* page = bpm_->FetchPage(0);
            if (page != nullptr) {
                // Data page me insert karo
                bool success = page->data_.InsertRecord(stmt.record_data);

                if (success) {
                    std::cout << "--> [Success] 1 row inserted into '" << stmt.table_name << "'.\n\n";
                    // Likhne ke baad page wapas rakho (Dirty = true, taaki disk pe save ho)
                    bpm_->UnpinPage(0, true);
                } else {
                    std::cout << "--> [Error] Page is full! Need a new page.\n\n";
                    bpm_->UnpinPage(0, false);
                }
            }
        }
    }

} // namespace dbengine