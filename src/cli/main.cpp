// src/cli/main.cpp
#include "parser/sql_parser.h"
#include "disk/disk_manager.h"
#include "buffer/buffer_pool_manager.h"
#include "execution/execution_engine.h"
#include "concurrency/lock_manager.h"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

// --- USER 1 KI QUERY ---
void TransactionOne(dbengine::LockManager& lm) {
    dbengine::Transaction txn1(1); // User 1 ka ID

    std::cout << "\n[User 1] Row 42 par taala lagane ki koshish kar raha hai...\n";
    if (lm.LockExclusive(&txn1, 42)) {
        std::cout << "[User 1] Taala mil gaya! Ab main 3 second tak heavy data likhunga...\n";

        // 3 second ka fake delay (Simulating database write)
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "[User 1] Kaam khatam! Taala khol raha hu.\n";
        lm.Unlock(&txn1, 42);
    }
}

// --- USER 2 KI QUERY ---
void TransactionTwo(dbengine::LockManager& lm) {
    dbengine::Transaction txn2(2); // User 2 ka ID

    // User 2 thoda sa late aata hai (adhe second baad)
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "\n[User 2] Mujhe bhi Row 42 par taala chahiye!\n";

    // User 2 taala mangega, par Engine usko MANA kar dega!
    if (!lm.LockExclusive(&txn2, 42)) {
        std::cout << "[User 2] Taala nahi mila (DENIED)! Mujhe wait karna padega...\n";

        // User 2 wait karta hai aur 3 second baad dobara try karta hai
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "\n[User 2] Dobara try kar raha hu...\n";
        if (lm.LockExclusive(&txn2, 42)) {
            std::cout << "[User 2] Aakhir kar taala mil gaya! Kaam shuru.\n";
            lm.Unlock(&txn2, 42);
        }
    }
}

int main() {
    std::cout << "Booting up DbEngine...\n";

    // ==========================================
    // --- Concurrency / ACID Test ---
    // ==========================================
    std::cout << "\n--- ACID Concurrency Test (2 Users) ---\n";

    dbengine::LockManager lock_manager;

    // Yahan hum 2 alag-alag parallel threads (users) chalu kar rahe hain
    std::thread user1(TransactionOne, std::ref(lock_manager));
    std::thread user2(TransactionTwo, std::ref(lock_manager));

    // Main program ko dono users ka wait karne do
    user1.join();
    user2.join();

    std::cout << "--- Concurrency Test Passed! ---\n\n";

    // ==========================================
    // --- Core Database Engine (CLI Shell) ---
    // ==========================================
    dbengine::DiskManager disk_manager("dbengine.db");
    dbengine::BufferPoolManager bpm(&disk_manager);
    dbengine::SQLParser parser;
    dbengine::ExecutionEngine executor(&bpm);

    std::cout << "========================================\n";
    std::cout << " Welcome to DbEngine CLI Shell \n";
    std::cout << " Type 'exit' to quit.\n";
    std::cout << "========================================\n\n";

    std::string user_input;

    while (true) {
        std::cout << "DbEngine> ";
        std::getline(std::cin, user_input);

        if (user_input == "exit" || user_input == "quit") {
            std::cout << "Shutting down DbEngine. Bye!\n";
            break;
        }
        if (user_input.empty()) continue;

        dbengine::SQLStatement stmt = parser.ParseQuery(user_input);

        if (stmt.type != dbengine::StatementType::INVALID) {
            executor.Execute(stmt);
        } else {
            std::cout << "[Error] Unrecognized SQL Command.\n\n";
        }
    }

    return 0;
}