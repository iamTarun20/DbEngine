// include/concurrency/lock_manager.h
#pragma once
#include "concurrency/transaction.h"
#include <mutex>
#include <unordered_map>
#include <iostream>

namespace dbengine {

    class LockManager {
    private:
        std::mutex latch_; // C++ ka actual asali taala (Traffic signal)

        // Kounse Row (RID) par kiska taala laga hai? (Row ID -> Transaction ID)
        std::unordered_map<int, int> lock_table_;

    public:
        LockManager() = default;

        // Likhne ke liye taala mangna (Exclusive Lock)
        bool LockExclusive(Transaction* txn, int row_id);

        // Kaam khatam hone par taala kholna
        void Unlock(Transaction* txn, int row_id);
    };

} // namespace dbengine