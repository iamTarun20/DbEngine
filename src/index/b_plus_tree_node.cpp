#include "index/b_plus_tree_node.h"
#include <stdexcept>
namespace dbengine {
BPlusTreeNode::BPlusTreeNode(bool is_leaf, int max_size)
    : is_leaf_(is_leaf), size_(0), max_size_(max_size), next_page_id_(-1) {
    if (max_size < 2 || max_size > 200) throw std::invalid_argument("Node capacity must be between 2 and 200");
}
bool BPlusTreeNode::IsLeaf() const { return is_leaf_; }
int BPlusTreeNode::GetSize() const { return size_; }
int BPlusTreeNode::FindValue(int key) const {
    int left = 0, right = size_;
    while (left < right) {
        const int middle = left + (right - left) / 2;
        if (keys_[middle] < key) left = middle + 1; else right = middle;
    }
    return left < size_ && keys_[left] == key ? values_[left] : -1;
}
bool BPlusTreeNode::Insert(int key, int value) {
    int position = 0;
    while (position < size_ && keys_[position] < key) ++position;
    if (size_ == max_size_ || (position < size_ && keys_[position] == key)) return false;
    for (int i = size_; i > position; --i) { keys_[i] = keys_[i - 1]; values_[i] = values_[i - 1]; }
    keys_[position] = key; values_[position] = value; ++size_;
    return true;
}
BPlusTreeNode* BPlusTreeNode::Split(int& promoted_key) {
    if (!is_leaf_ || size_ < 2) throw std::logic_error("Only non-empty leaf nodes can be split");
    auto* sibling = new BPlusTreeNode(true, max_size_);
    const int middle = size_ / 2;
    promoted_key = keys_[middle];
    for (int i = middle; i < size_; ++i) sibling->Insert(keys_[i], values_[i]);
    size_ = middle;
    sibling->next_page_id_ = next_page_id_;
    return sibling;
}
}
