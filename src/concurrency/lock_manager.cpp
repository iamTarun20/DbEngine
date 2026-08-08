// src/concurrency/lock_manager.cpp
#include "concurrency/lock_manager.h"

namespace dbengine {

    bool LockManager::LockExclusive(Transaction* txn, int row_id) {
        // std::lock_guard automatically latch_ ko lock kar deta hai aur function ke end pe khol deta hai
        // Taaki lock_table_ modify karte waqt koi aur usme na ghuse
        std::lock_guard<std::mutex> lock(latch_);

        // Check karo kya kisi aur ne is row par taala lagaya hua hai?
        if (lock_table_.find(row_id) != lock_table_.end()) {
            int owner_txn = lock_table_[row_id];
            if (owner_txn != txn->GetId()) {
                std::cout << "[LockManager] Txn " << txn->GetId() << " DENIED! Row "
                          << row_id << " is already locked by Txn " << owner_txn << ".\n";
                return false; // Taala nahi mila, wapas jao!
            }
        }

        // Agar khali hai, toh apna taala laga lo
        lock_table_[row_id] = txn->GetId();
        std::cout << "[LockManager] Txn " << txn->GetId() << " ACQUIRED Exclusive Lock on Row " << row_id << ".\n";
        return true;
    }

    void LockManager::Unlock(Transaction* txn, int row_id) {
        std::lock_guard<std::mutex> lock(latch_);

        if (lock_table_.find(row_id) != lock_table_.end() && lock_table_[row_id] == txn->GetId()) {
            lock_table_.erase(row_id); // Apna taala hata lo
            std::cout << "[LockManager] Txn " << txn->GetId() << " RELEASED Lock on Row " << row_id << ".\n";
        }
    }

} // namespace dbengine