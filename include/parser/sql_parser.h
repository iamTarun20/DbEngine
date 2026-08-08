// include/parser/sql_parser.h
#pragma once
#include <string>
#include <vector>

namespace dbengine {

    // Engine ko samajh aane wale Action Types
    enum class StatementType {
        SELECT,
        INSERT,
        INVALID
    };

    // Yeh wo C++ Object hai jo Parser banakar Executor ko dega (AST)
    struct SQLStatement {
        StatementType type = StatementType::INVALID;
        std::string table_name;
        std::string record_data; // INSERT ke liye data
    };

    class SQLParser {
    private:
        // Lexer: String ko words (tokens) mein todne ka tool
        std::vector<std::string> Tokenize(const std::string& query);

    public:
        // Main function jo string lega aur SQLStatement dega
        SQLStatement ParseQuery(const std::string& query);
    };

} // namespace dbengine