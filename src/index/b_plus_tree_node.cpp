// src/index/b_plus_tree_node.cpp
#include "index/b_plus_tree_node.h"

namespace dbengine {

    BPlusTreeNode::BPlusTreeNode(bool is_leaf, int max_size)
        : is_leaf_(is_leaf), size_(0), max_size_(max_size), next_page_id_(-1) {}

    bool BPlusTreeNode::IsLeaf() const { return is_leaf_; }
    int BPlusTreeNode::GetSize() const { return size_; }

    int BPlusTreeNode::FindValue(int search_key) const {
        int left = 0;
        int right = size_ - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;

            if (keys_[mid] == search_key) {
                return values_[mid];
            } else if (keys_[mid] < search_key) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return -1;
    }

    bool BPlusTreeNode::Insert(int key, int value) {
        if (size_ >= max_size_) {
            return false;
        }

        int insert_index = 0;
        while (insert_index < size_ && keys_[insert_index] < key) {
            insert_index++;
        }

        for (int i = size_; i > insert_index; i--) {
            keys_[i] = keys_[i - 1];
            values_[i] = values_[i - 1];
        }

        keys_[insert_index] = key;
        values_[insert_index] = value;
        size_++;

        return true;
    }

    // YEH RAHA SPLIT KA LOGIC
    BPlusTreeNode* BPlusTreeNode::Split(int& out_promoted_key) {
        BPlusTreeNode* new_sibling = new BPlusTreeNode(this->is_leaf_, this->max_size_);

        int mid_index = this->size_ / 2;
        out_promoted_key = this->keys_[mid_index];

        int j = 0;
        for (int i = mid_index; i < this->size_; i++) {
            new_sibling->keys_[j] = this->keys_[i];
            new_sibling->values_[j] = this->values_[i];
            j++;
        }

        new_sibling->size_ = j;
        this->size_ = mid_index;

        if (this->is_leaf_) {
            new_sibling->next_page_id_ = this->next_page_id_;
        }

        return new_sibling;
    }

} // namespace dbengine