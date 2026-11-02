#pragma once
#include "concurrency/transaction.h"
#include <mutex>
#include <unordered_map>
namespace dbengine {
// A standalone, non-blocking lock demonstration; not connected to query execution.
class LockManager {
    std::mutex mutex_;
    std::unordered_map<int, int> lock_table_;
public:
    bool LockExclusive(Transaction* transaction, int row_id);
    void Unlock(Transaction* transaction, int row_id);
};
}
