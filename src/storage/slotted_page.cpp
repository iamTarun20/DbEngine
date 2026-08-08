#include "storage/slotted_page.h"
#include <cstring>

namespace dbengine {

    SlottedPage::SlottedPage() {
        std::memset(page_data_, 0, PAGE_SIZE);
        SetRecordCount(0);
        SetFreeSpacePointer(PAGE_SIZE);
    }

    uint16_t SlottedPage::GetRecordCount() const {
        return *reinterpret_cast<const uint16_t*>(page_data_ + 0);
    }

    void SlottedPage::SetRecordCount(uint16_t count) {
        *reinterpret_cast<uint16_t*>(page_data_ + 0) = count;
    }

    uint16_t SlottedPage::GetFreeSpacePointer() const {
        return *reinterpret_cast<const uint16_t*>(page_data_ + 2);
    }

    void SlottedPage::SetFreeSpacePointer(uint16_t ptr) {
        *reinterpret_cast<uint16_t*>(page_data_ + 2) = ptr;
    }

    bool SlottedPage::InsertRecord(const std::string& record_data) {
        uint16_t record_size = record_data.size();
        uint16_t free_space_ptr = GetFreeSpacePointer();
        uint16_t record_count = GetRecordCount();

        uint16_t directory_end = 4 + (record_count + 1) * sizeof(Slot);
        uint16_t new_free_space_ptr = free_space_ptr - record_size;

        if (directory_end > new_free_space_ptr) {
            return false;
        }

        std::memcpy(page_data_ + new_free_space_ptr, record_data.c_str(), record_size);

        Slot new_slot;
        new_slot.offset = new_free_space_ptr;
        new_slot.size = record_size;

        uint16_t slot_location = 4 + (record_count * sizeof(Slot));
        std::memcpy(page_data_ + slot_location, &new_slot, sizeof(Slot));

        SetFreeSpacePointer(new_free_space_ptr);
        SetRecordCount(record_count + 1);

        return true;
    }

    std::string SlottedPage::GetRecord(uint16_t slot_id) const {
        if (slot_id >= GetRecordCount()) return "";
        uint16_t slot_location = 4 + (slot_id * sizeof(Slot));
        const Slot* slot = reinterpret_cast<const Slot*>(page_data_ + slot_location);
        return std::string(page_data_ + slot->offset, slot->size);
    }

    const char* SlottedPage::GetRawData() const {
        return page_data_;
    }

}