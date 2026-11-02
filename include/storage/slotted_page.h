#pragma once
#include "common/config.h"
#include <cstdint>
#include <string>

namespace dbengine {
struct Slot { uint16_t offset; uint16_t size; };

class SlottedPage {
    char page_data_[PAGE_SIZE];
public:
    SlottedPage();
    uint16_t GetRecordCount() const;
    void SetRecordCount(uint16_t count);
    uint16_t GetFreeSpacePointer() const;
    void SetFreeSpacePointer(uint16_t offset);
    bool IsValid() const;
    bool InsertRecord(const std::string& record);
    std::string GetRecord(uint16_t slot_id) const;
    const char* GetRawData() const;
    char* GetRawData();
};
}
