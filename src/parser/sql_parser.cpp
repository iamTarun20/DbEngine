// src/parser/sql_parser.cpp
#include "parser/sql_parser.h"
#include <sstream>

namespace dbengine {

    std::vector<std::string> SQLParser::Tokenize(const std::string& query) {
        std::vector<std::string> tokens;
        std::stringstream ss(query);
        std::string word;

        // Space ke basis pe string ko words me todna
        while (ss >> word) {
            tokens.push_back(word);
        }
        return tokens;
    }

    SQLStatement SQLParser::ParseQuery(const std::string& query) {
        SQLStatement stmt;
        std::vector<std::string> tokens = Tokenize(query);

        if (tokens.empty()) return stmt;

        // 1. SELECT Query parse karna
        // Example: SELECT * FROM student
        if (tokens[0] == "SELECT" || tokens[0] == "select") {
            stmt.type = StatementType::SELECT;

            // Basic check for table name
            for (size_t i = 0; i < tokens.size(); i++) {
                if ((tokens[i] == "FROM" || tokens[i] == "from") && i + 1 < tokens.size()) {
                    stmt.table_name = tokens[i + 1];
                    break;
                }
            }
        }
        // 2. INSERT Query parse karna
        // Example: INSERT INTO student VALUES Tarun_Kumar_IT_Branch
        else if (tokens[0] == "INSERT" || tokens[0] == "insert") {
            stmt.type = StatementType::INSERT;

            for (size_t i = 0; i < tokens.size(); i++) {
                if ((tokens[i] == "INTO" || tokens[i] == "into") && i + 1 < tokens.size()) {
                    stmt.table_name = tokens[i + 1];
                }
                if ((tokens[i] == "VALUES" || tokens[i] == "values") && i + 1 < tokens.size()) {
                    stmt.record_data = tokens[i + 1];
                    // Note: Abhi hum single string as value le rahe hain simplicity ke liye
                }
            }
        }

        return stmt;
    }

} // namespace dbengine