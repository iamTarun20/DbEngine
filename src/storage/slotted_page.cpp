#include "storage/slotted_page.h"
#include <cstring>
#include <stdexcept>

namespace dbengine {
namespace { constexpr size_t HEADER_SIZE = 4; }
SlottedPage::SlottedPage() : page_data_{} { SetFreeSpacePointer(PAGE_SIZE); }
uint16_t SlottedPage::GetRecordCount() const {
    uint16_t count; std::memcpy(&count, page_data_, sizeof(count)); return count;
}
void SlottedPage::SetRecordCount(uint16_t count) { std::memcpy(page_data_, &count, sizeof(count)); }
uint16_t SlottedPage::GetFreeSpacePointer() const {
    uint16_t offset; std::memcpy(&offset, page_data_ + 2, sizeof(offset)); return offset;
}
void SlottedPage::SetFreeSpacePointer(uint16_t offset) { std::memcpy(page_data_ + 2, &offset, sizeof(offset)); }
bool SlottedPage::IsValid() const {
    const size_t count = GetRecordCount();
    const size_t free_offset = GetFreeSpacePointer();
    const size_t directory_end = HEADER_SIZE + count * sizeof(Slot);
    if (directory_end > PAGE_SIZE || free_offset < directory_end || free_offset > PAGE_SIZE) return false;
    for (size_t i = 0; i < count; ++i) {
        Slot slot; std::memcpy(&slot, page_data_ + HEADER_SIZE + i * sizeof(Slot), sizeof(slot));
        if (slot.offset < free_offset || static_cast<size_t>(slot.offset) + slot.size > PAGE_SIZE) return false;
    }
    return true;
}
bool SlottedPage::InsertRecord(const std::string& record) {
    if (!IsValid()) throw std::runtime_error("Invalid page contents");
    const size_t count = GetRecordCount();
    const size_t free_offset = GetFreeSpacePointer();
    const size_t directory_end = HEADER_SIZE + (count + 1) * sizeof(Slot);
    if (directory_end > free_offset || record.size() > free_offset - directory_end) return false;
    const auto new_offset = static_cast<uint16_t>(free_offset - record.size());
    std::memcpy(page_data_ + new_offset, record.data(), record.size());
    const Slot slot{new_offset, static_cast<uint16_t>(record.size())};
    std::memcpy(page_data_ + HEADER_SIZE + count * sizeof(Slot), &slot, sizeof(slot));
    SetFreeSpacePointer(new_offset);
    SetRecordCount(static_cast<uint16_t>(count + 1));
    return true;
}
std::string SlottedPage::GetRecord(uint16_t slot_id) const {
    if (!IsValid()) throw std::runtime_error("Invalid page contents");
    if (slot_id >= GetRecordCount()) throw std::out_of_range("Invalid slot ID");
    Slot slot; std::memcpy(&slot, page_data_ + HEADER_SIZE + slot_id * sizeof(Slot), sizeof(slot));
    return std::string(page_data_ + slot.offset, slot.size);
}
const char* SlottedPage::GetRawData() const { return page_data_; }
char* SlottedPage::GetRawData() { return page_data_; }
}
