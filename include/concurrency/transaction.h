#pragma once
namespace dbengine {
// States used by the standalone lock example. Rollback is not implemented.
enum class TxnState { GROWING, SHRINKING, COMMITTED, ABORTED };
class Transaction {
    int transaction_id_;
    TxnState state_ = TxnState::GROWING;
public:
    explicit Transaction(int id) : transaction_id_(id) {}
    int GetId() const { return transaction_id_; }
    TxnState GetState() const { return state_; }
    void SetState(TxnState state) { state_ = state; }
};
}
