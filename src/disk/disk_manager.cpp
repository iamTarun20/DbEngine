#include "disk/disk_manager.h"
#include <limits>
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

    db_file_.seekg(0, std::ios::end);
    const std::streamoff file_size = db_file_.tellg();
    if (file_size < 0) throw std::runtime_error("Cannot determine database size");
    if (file_size % PAGE_SIZE != 0) throw std::runtime_error("Truncated database page");
    if (file_size / PAGE_SIZE > std::numeric_limits<page_id_t>::max())
        throw std::runtime_error("Database has too many pages");
    page_count_ = static_cast<page_id_t>(file_size / PAGE_SIZE);
}
void DiskManager::WritePage(page_id_t page_id, const char* data) {
    if (page_id < 0 || !data) throw std::invalid_argument("Invalid page write");
    if (page_id > page_count_) throw std::invalid_argument("Database pages must be allocated in order");
    if (page_id == std::numeric_limits<page_id_t>::max())
        throw std::runtime_error("Database page limit reached");
    db_file_.clear();
    db_file_.seekp(static_cast<std::streamoff>(page_id) * PAGE_SIZE);
    db_file_.write(data, PAGE_SIZE);
    db_file_.flush();
    if (!db_file_) throw std::runtime_error("Failed to write database page");
    if (page_id == page_count_) ++page_count_;
}
bool DiskManager::ReadPage(page_id_t page_id, char* data) {
    if (page_id < 0 || !data) throw std::invalid_argument("Invalid page read");
    if (page_id >= page_count_) return false;
    db_file_.clear();
    db_file_.seekg(static_cast<std::streamoff>(page_id) * PAGE_SIZE);
    db_file_.read(data, PAGE_SIZE);
    if (!db_file_) throw std::runtime_error("Failed to read database page");
    return true;
}
}
