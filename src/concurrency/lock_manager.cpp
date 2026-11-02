#include "concurrency/lock_manager.h"
namespace dbengine {
bool LockManager::LockExclusive(Transaction* transaction, int row_id) {
    if (!transaction) return false;
    std::lock_guard<std::mutex> guard(mutex_);
    auto found = lock_table_.find(row_id);
    if (found != lock_table_.end()) return found->second == transaction->GetId();
    if (transaction->GetState() != TxnState::GROWING) return false;
    lock_table_[row_id] = transaction->GetId();
    return true;
}
void LockManager::Unlock(Transaction* transaction, int row_id) {
    if (!transaction) return;
    std::lock_guard<std::mutex> guard(mutex_);
    auto found = lock_table_.find(row_id);
    if (found != lock_table_.end() && found->second == transaction->GetId()) {
        lock_table_.erase(found);
        if (transaction->GetState() == TxnState::GROWING) transaction->SetState(TxnState::SHRINKING);
    }
}
}
