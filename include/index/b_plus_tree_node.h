// include/index/b_plus_tree_node.h
#pragma once
#include "common/config.h"

namespace dbengine {

    struct RID {
        page_id_t page_id;
        int slot_id;
    };

    class BPlusTreeNode {
    private:
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

        int FindValue(int search_key) const;
        bool Insert(int key, int value);

        // YEH RAHI MISSING DECLARATION!
        BPlusTreeNode* Split(int& out_promoted_key);
    };

} // namespace dbengine