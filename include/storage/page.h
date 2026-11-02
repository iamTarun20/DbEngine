#pragma once
#include "storage/slotted_page.h"
namespace dbengine {
class Page {
public:
    SlottedPage data_;
    page_id_t page_id_ = -1;
    int pin_count_ = 0;
    bool is_dirty_ = false;
    void ResetMemory() {
        page_id_ = -1;
        pin_count_ = 0;
        is_dirty_ = false;
        data_ = SlottedPage();
    }
};
}
