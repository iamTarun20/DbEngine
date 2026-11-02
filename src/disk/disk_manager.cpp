#include "disk/disk_manager.h"
#include <stdexcept>
namespace dbengine {
DiskManager::DiskManager(const std::string& file_name) {
    db_file_.open(file_name, std::ios::binary | std::ios::in | std::ios::out);
    if (!db_file_.is_open()) {
        std::ofstream create(file_name, std::ios::binary | std::ios::app);
        if (!create) throw std::runtime_error("Cannot create database file: " + file_name);
        create.close();
        db_file_.open(file_name, std::ios::binary | std::ios::in | std::ios::out);
    }
    if (!db_file_) throw std::runtime_error("Cannot open database file: " + file_name);
}
void DiskManager::WritePage(page_id_t page_id, const char* data) {
    if (page_id < 0 || !data) throw std::invalid_argument("Invalid page write");
    db_file_.clear();
    db_file_.seekp(static_cast<std::streamoff>(page_id) * PAGE_SIZE);
    db_file_.write(data, PAGE_SIZE);
    db_file_.flush();
    if (!db_file_) throw std::runtime_error("Failed to write database page");
}
bool DiskManager::ReadPage(page_id_t page_id, char* data) {
    if (page_id < 0 || !data) throw std::invalid_argument("Invalid page read");
    db_file_.clear();
    db_file_.seekg(0, std::ios::end);
    const auto file_size = db_file_.tellg();
    if (file_size < 0) throw std::runtime_error("Cannot determine database size");
    const auto offset = static_cast<std::streamoff>(page_id) * PAGE_SIZE;
    if (offset >= file_size) return false;
    if (file_size - offset < PAGE_SIZE) throw std::runtime_error("Truncated database page");
    db_file_.seekg(offset);
    db_file_.read(data, PAGE_SIZE);
    if (!db_file_) throw std::runtime_error("Failed to read database page");
    return true;
}
}
