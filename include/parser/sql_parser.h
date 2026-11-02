#pragma once
#include <string>
namespace dbengine {
enum class StatementType { SELECT, INSERT, INVALID };
struct SQLStatement {
    StatementType type = StatementType::INVALID;
    std::string table_name;
    std::string record_data;
};
class SQLParser {
public:
    SQLStatement ParseQuery(const std::string& query);
};
}
