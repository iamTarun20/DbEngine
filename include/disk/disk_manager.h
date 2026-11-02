#pragma once
#include "common/config.h"
#include <fstream>
#include <string>
namespace dbengine {
class DiskManager {
    std::fstream db_file_;
public:
    explicit DiskManager(const std::string& file_name);
    void WritePage(page_id_t page_id, const char* data);
    // Returns false when the page is beyond the end of the file.
    bool ReadPage(page_id_t page_id, char* data);
};
}
