#include "parser/sql_parser.h"
#include "disk/disk_manager.h"
#include "buffer/buffer_pool_manager.h"
#include "execution/execution_engine.h"
#include <exception>
#include <iostream>
#include <string>
int main() {
    try {
        dbengine::DiskManager disk("dbengine.db");
        dbengine::BufferPoolManager buffer_pool(&disk);
        dbengine::SQLParser parser;
        dbengine::ExecutionEngine executor(&buffer_pool);
        std::cout << "DbEngine: type exit to quit.\n";
        std::string input;
        while (std::cout << "DbEngine> " && std::getline(std::cin, input)) {
            if (input == "exit" || input == "quit") break;
            if (input.empty()) continue;
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
