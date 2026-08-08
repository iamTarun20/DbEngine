#pragma once
#include "common/config.h"
#include <fstream>
#include <string>

namespace dbengine {
    class DiskManager {
    private:
        std::fstream db_file_;
        std::string file_name_;
    public:
        DiskManager(const std::string& db_file);
        ~DiskManager();
        void WritePage(page_id_t page_id, const char* page_data);
        void ReadPage(page_id_t page_id, char* page_data);
    };
}