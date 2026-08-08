// include/concurrency/transaction.h
#pragma once

namespace dbengine {

    // Ek query kis halat me hai?
    enum class TxnState {
        GROWING,    // Taale ikkathe kar raha hai
        SHRINKING,  // Taale khol raha hai
        COMMITTED,  // Kaam success ho gaya
        ABORTED     // Fail ho gaya (Rollback)
    };

    class Transaction {
    private:
        int txn_id_;
        TxnState state_;

    public:
        Transaction(int id) : txn_id_(id), state_(TxnState::GROWING) {}

        int GetId() const { return txn_id_; }
        TxnState GetState() const { return state_; }
        void SetState(TxnState state) { state_ = state; }
    };

} // namespace dbengine