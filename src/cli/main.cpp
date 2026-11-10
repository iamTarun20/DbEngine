#include "parser/sql_parser.h"
#include "disk/disk_manager.h"
#include "buffer/buffer_pool_manager.h"
#include "execution/execution_engine.h"
#include <exception>
#include <iostream>
#include <string>
#include <string_view>

namespace {
constexpr char USAGE[] = "Usage: DbEngine [--batch] [database-file]\n";
}

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        std::cout << USAGE;
        return 0;
    }
    const bool batch = argc > 1 && std::string_view(argv[1]) == "--batch";
    const int path_arg = batch ? 2 : 1;
    if (argc > path_arg + 1) {
        std::cerr << USAGE;
        return 2;
    }
    try {
        dbengine::DiskManager disk(argc > path_arg ? argv[path_arg] : "dbengine.db");
        dbengine::BufferPoolManager buffer_pool(&disk);
        dbengine::SQLParser parser;
        dbengine::ExecutionEngine executor(&buffer_pool);
        if (!batch) std::cout << "DbEngine (type .stats for cache counters or exit to quit)\n";
        std::string input;
        for (;;) {
            if (!batch) std::cout << "DbEngine> ";
            if (!std::getline(std::cin, input)) break;
            if (input == "exit" || input == "quit") break;
            if (input.empty()) continue;
            if (input == ".stats") {
                const auto stats = buffer_pool.GetStats();
                std::cout << "pages=" << buffer_pool.GetPageCount()
                          << " cache_hits=" << stats.cache_hits
                          << " cache_misses=" << stats.cache_misses
                          << " evictions=" << stats.evictions
                          << " page_reads=" << stats.page_reads
                          << " page_writes=" << stats.page_writes << '\n';
                continue;
            }
            auto statement = parser.ParseQuery(input);
            if (statement.type == dbengine::StatementType::INVALID)
                std::cout << "Invalid query. See README for supported syntax.\n";
            else executor.Execute(statement);
        }
        buffer_pool.FlushAllPages();
    } catch (const std::exception& error) {
        std::cerr << "DbEngine error: " << error.what() << '\n';
        return 1;
    }
}
