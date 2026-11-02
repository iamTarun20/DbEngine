#include "execution/execution_engine.h"
#include <iostream>
#include <stdexcept>
namespace dbengine {
ExecutionEngine::ExecutionEngine(BufferPoolManager* buffer_pool) : buffer_pool_(buffer_pool) {
    if (!buffer_pool_) throw std::invalid_argument("Buffer pool is required");
}
void ExecutionEngine::Execute(const SQLStatement& statement) {
    if (statement.type == StatementType::INVALID || statement.table_name.empty()) {
        std::cout << "Invalid statement.\n"; return;
    }
    Page* page = buffer_pool_->FetchPage(0);
    if (!page) { std::cout << "No buffer frame available.\n"; return; }
    // A table name and a tab prefix each record. Names are validated by the parser.
    const std::string prefix = statement.table_name + "\t";
    if (statement.type == StatementType::INSERT) {
        const bool inserted = page->data_.InsertRecord(prefix + statement.record_data);
        buffer_pool_->UnpinPage(0, inserted);
        std::cout << (inserted ? "1 row inserted.\n" : "Page is full; row not inserted.\n");
    } else {
        int count = 0;
        for (uint16_t slot = 0; slot < page->data_.GetRecordCount(); ++slot) {
            const std::string record = page->data_.GetRecord(slot);
            if (record.compare(0, prefix.size(), prefix) == 0) {
                std::cout << record.substr(prefix.size()) << '\n'; ++count;
            }
        }
        buffer_pool_->UnpinPage(0, false);
        std::cout << count << " row(s) returned.\n";
    }
}
}
