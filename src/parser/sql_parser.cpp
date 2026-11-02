#include "parser/sql_parser.h"
#include <algorithm>
#include <cctype>
#include <regex>
namespace dbengine {
SQLStatement SQLParser::ParseQuery(const std::string& query) {
    static const std::regex select_pattern(R"(^\s*SELECT\s+\*\s+FROM\s+([A-Za-z_][A-Za-z0-9_]*)\s*;?\s*$)", std::regex::icase);
    static const std::regex insert_pattern(R"(^\s*INSERT\s+INTO\s+([A-Za-z_][A-Za-z0-9_]*)\s+VALUES\s+(?:'([^']*)'|([^\s;']+))\s*;?\s*$)", std::regex::icase);
    std::smatch match;
    SQLStatement statement;
    if (std::regex_match(query, match, select_pattern)) statement.type = StatementType::SELECT;
    else if (std::regex_match(query, match, insert_pattern)) {
        statement.type = StatementType::INSERT;
        statement.record_data = match[2].matched ? match[2].str() : match[3].str();
    } else return statement;
    statement.table_name = match[1];
    std::transform(statement.table_name.begin(), statement.table_name.end(), statement.table_name.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return statement;
}
}
