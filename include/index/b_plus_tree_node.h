#pragma once
#include "common/config.h"
namespace dbengine {
struct RID { page_id_t page_id; int slot_id; };
// Experimental in-memory leaf node, not a complete or persistent B+ tree.
class BPlusTreeNode {
    bool is_leaf_;
    int size_;
    int max_size_;
    page_id_t next_page_id_;
    int keys_[200];
    int values_[200];
public:
    BPlusTreeNode(bool is_leaf, int max_size);
    bool IsLeaf() const;
    int GetSize() const;
    int FindValue(int search_key) const; // Returns -1 for a missing key.
    bool Insert(int key, int value);
    // Caller owns the returned sibling; internal nodes are not implemented.
    BPlusTreeNode* Split(int& promoted_key);
};
}
