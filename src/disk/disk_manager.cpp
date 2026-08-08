// src/disk/disk_manager.cpp
#include "disk/disk_manager.h"

namespace dbengine {

    DiskManager::DiskManager(const std::string& db_file) : file_name_(db_file) {
        db_file_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);
        if (!db_file_.is_open()) {
            db_file_.clear();
            db_file_.open(file_name_, std::ios::binary | std::ios::trunc | std::ios::out);
            db_file_.close();
            db_file_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);
        }
    }

    DiskManager::~DiskManager() {
        if (db_file_.is_open()) {
            db_file_.close();
        }
    }

    void DiskManager::WritePage(page_id_t page_id, const char* page_data) {
        db_file_.clear(); // <--- YEH LINE ZAROORI HAI (Error flags clear karne ke liye)
        int offset = page_id * PAGE_SIZE;
        db_file_.seekp(offset);
        db_file_.write(page_data, PAGE_SIZE);
        db_file_.flush();
    }

    void DiskManager::ReadPage(page_id_t page_id, char* page_data) {
        db_file_.clear(); // <--- YEH LINE ZAROORI HAI
        int offset = page_id * PAGE_SIZE;
        db_file_.seekg(offset);
        db_file_.read(page_data, PAGE_SIZE);
    }

} // namespace dbengine