// include/execution/execution_engine.h
#pragma once
#include "parser/sql_parser.h"
#include "buffer/buffer_pool_manager.h"
#include <iostream>

namespace dbengine {

    class ExecutionEngine {
    private:
        BufferPoolManager* bpm_;

    public:
        ExecutionEngine(BufferPoolManager* bpm);

        // Ye function Parser se 'SQLStatement' lega aur actual data read/write karega
        void Execute(const SQLStatement& stmt);
    };

} // namespace dbengine