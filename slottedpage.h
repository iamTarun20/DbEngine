// include/storage/slotted_page.hpp
#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>

constexpr int PAGE_SIZE = 4096;

// Ek Slot ki entry - ye batayega data kahan hai aur kitna bada hai
struct Slot {
    uint16_t offset; // Neeche se data kahan rakha hai (e.g., 4080)
    uint16_t size;   // Data ka size kya hai (e.g., 15 bytes)
};

class SlottedPage {
private:
    char page_data_[PAGE_SIZE]; // Wahi hamara 4KB ka dabba

public:
    // Constructor: Page ko completely empty format me setup karna
    SlottedPage() {
        // Page ko zero se saaf kar do
        std::memset(page_data_, 0, PAGE_SIZE);

        // Shuru ke bytes me hum apna header set karenge
        SetRecordCount(0);
        SetFreeSpacePointer(PAGE_SIZE); // Shuru me poora 4096 bytes free hai
    }

    // --- Getters & Setters for Header ---

    // Page ke top pe record count save/read karna
    uint16_t GetRecordCount() {
        return *reinterpret_cast<uint16_t*>(page_data_ + 0); // Byte 0-1
    }
    void SetRecordCount(uint16_t count) {
        *reinterpret_cast<uint16_t*>(page_data_ + 0) = count;
    }

    // Free space kahan se shuru hai, uska pointer
    uint16_t GetFreeSpacePointer() {
        return *reinterpret_cast<uint16_t*>(page_data_ + 2); // Byte 2-3
    }
    void SetFreeSpacePointer(uint16_t ptr) {
        *reinterpret_cast<uint16_t*>(page_data_ + 2) = ptr;
    }

    // --- Core Operations ---

    // Naya data page me daalna
    bool InsertRecord(const char* record_data, uint16_t record_size) {
        uint16_t free_space_ptr = GetFreeSpacePointer();
        uint16_t record_count = GetRecordCount();

        // Check karo ki dabe me jagah bachi hai ya nahi?
        // Header (4 bytes) + (Har slot ka size 4 bytes * naye total records)
        uint16_t directory_end = 4 + (record_count + 1) * sizeof(Slot);
        uint16_t new_free_space_ptr = free_space_ptr - record_size;

        if (directory_end > new_free_space_ptr) {
            return false; // PAGE FULL! Ab naya box lana padega
        }

        // 1. Data ko bottom me copy karo (Neeche se upar)
        std::memcpy(page_data_ + new_free_space_ptr, record_data, record_size);

        // 2. Directory me ek naya Slot (Entry) banao (Upar se neeche)
        Slot new_slot;
        new_slot.offset = new_free_space_ptr;
        new_slot.size = record_size;

        // Slot ko sahi jagah par likh do
        uint16_t slot_location = 4 + (record_count * sizeof(Slot));
        std::memcpy(page_data_ + slot_location, &new_slot, sizeof(Slot));

        // 3. Header ko update kardo
        SetFreeSpacePointer(new_free_space_ptr);
        SetRecordCount(record_count + 1);

        return true;
    }

    // Slot number doge toh data nikal aayega
    void GetRecord(uint16_t slot_id, char* output_buffer) {
        if (slot_id >= GetRecordCount()) return; // Invalid slot

        // Directory se slot padho
        uint16_t slot_location = 4 + (slot_id * sizeof(Slot));
        Slot* slot = reinterpret_cast<Slot*>(page_data_ + slot_location);

        // Asli data location se utha kar output buffer me daal do
        std::memcpy(output_buffer, page_data_ + slot->offset, slot->size);
        output_buffer[slot->size] = '\0'; // String ko terminate karne ke liye
    }

    // (Helper) Jisse hum apni raw 4KB memory disk pe bhej sakein
    const char* GetRawData() const { return page_data_; }
};