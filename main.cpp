// src/main.cpp
#include <iostream>
#include <cstring>
#include "disk_manager.h"

int main() {
    std::cout << "Starting NebulaDB Disk Manager Test..." << std::endl;

    // 1. Dukaandar ko bulao aur usko bolo "nebuladb.db" naam ka godown sambhale
    DiskManager disk_manager("nebuladb.db");

    // 2. RAM (Memory) mein 4KB ka ek khaali array (box) banao
    char my_write_box[PAGE_SIZE] = {0}; // Sab kuch 0 se bhar diya

    // Us box mein kuch data daal do
    std::strcpy(my_write_box, "Hello NebulaDB! Ye data seedha Hard Disk pe jayega.");

    // 3. Disk Manager ko bolo: "Bhai, ye box lo aur isko Page ID 0 par save kardo"
    std::cout << "Writing to disk..." << std::endl;
    disk_manager.WritePage(0, my_write_box);

    // 4. Ab check karte hain. Ek naya khaali box banao RAM mein.
    char my_read_box[PAGE_SIZE] = {0};

    // Disk Manager ko bolo: "Page 0 se wapas padh ke lao aur is naye box mein dalo"
    std::cout << "Reading from disk..." << std::endl;
    disk_manager.ReadPage(0, my_read_box);

    // 5. Output check karo
    std::cout << "Disk se ye data mila: " << my_read_box << std::endl;

    return 0;
}