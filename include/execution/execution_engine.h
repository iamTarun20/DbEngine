#pragma once
#include "parser/sql_parser.h"
#include "buffer/buffer_pool_manager.h"
namespace dbengine {
class ExecutionEngine {
    BufferPoolManager* buffer_pool_;
public:
    explicit ExecutionEngine(BufferPoolManager* buffer_pool);
    void Execute(const SQLStatement& statement);
};
}
