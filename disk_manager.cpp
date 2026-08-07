// src/disk/disk_manager.cpp
#include "disk_manager.h"
#include <iostream>
#include <cstring>

// Constructor
DiskManager::DiskManager(const std::string& db_file) : file_name_(db_file) {
    // CONCEPT: Binary Mode
    // Hum file ko text (.txt) ki tarah nahi, binary (1s and 0s) ki tarah kholenge.
    // 'in' matlab Read, 'out' matlab Write.
    db_file_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);

    // Agar file pehle se exist nahi karti (pehli baar run ho raha hai)
    if (!db_file_.is_open()) {
        db_file_.clear();
        // Nayi file bana do
        db_file_.open(file_name_, std::ios::binary | std::ios::trunc | std::ios::out);
        db_file_.close();
        // Wapas dono mode (read/write) me kholo
        db_file_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);
    }
}

DiskManager::~DiskManager() {
    if (db_file_.is_open()) {
        db_file_.close(); // Dukaan band, file close!
    }
}

void DiskManager::WritePage(int page_id, const char* page_data) {
    // CONCEPT: Offset
    // Agar Page 0 likhna hai, toh file me 0 byte pe jao (0 * 4096)
    // Agar Page 1 likhna hai, toh file me 4096th byte pe jao (1 * 4096)
    int offset = page_id * PAGE_SIZE;

    // Cursor ko us exact jagah le jao jahan likhna hai
    db_file_.seekp(offset);

    // Poora 4KB data ek jhatke me likh do
    db_file_.write(page_data, PAGE_SIZE);

    // Forcefully disk pe utaar do (taaki OS delay na kare)
    db_file_.flush();
}

void DiskManager::ReadPage(int page_id, char* page_data) {
    int offset = page_id * PAGE_SIZE;

    // Cursor ko us page ki shuruaat par le jao
    db_file_.seekg(offset);

    // Wahan se 4KB data uthakar humare memory (page_data) me daal do
    db_file_.read(page_data, PAGE_SIZE);
}