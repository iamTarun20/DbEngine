// include/disk/disk_manager.hpp
#pragma once
#include <fstream>
#include <string>

// Yeh wahi constant hai jo humne socha tha
constexpr int PAGE_SIZE = 4096;

class DiskManager {
private:
    std::fstream db_file_; // File handle (Iske through hum OS se baat karenge)
    std::string file_name_;

public:
    // Constructor: Jab manager banega, tab file open karega
    DiskManager(const std::string& db_file);

    // Destructor: Jab kaam khatam, file close karega
    ~DiskManager();

    // 4KB ka dabba disk pe likhne ka function
    void WritePage(int page_id, const char* page_data);

    // 4KB ka dabba disk se wapas RAM me laane ka function
    void ReadPage(int page_id, char* page_data);
};