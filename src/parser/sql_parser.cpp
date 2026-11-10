#include "parser/sql_parser.h"
#include <algorithm>
#include <cctype>
#include <string_view>

namespace dbengine {
namespace {
constexpr std::string_view WHITESPACE = " \t\r\n\f\v";

std::string_view Trim(std::string_view input) {
    const auto first = input.find_first_not_of(WHITESPACE);
    if (first == std::string_view::npos) return {};
    return input.substr(first, input.find_last_not_of(WHITESPACE) - first + 1);
}

std::string_view TakeToken(std::string_view& input) {
    const auto first = input.find_first_not_of(WHITESPACE);
    if (first == std::string_view::npos) {
        input = {};
        return {};
    }
    input.remove_prefix(first);
    const auto token = input.substr(0, input.find_first_of(WHITESPACE));
    input.remove_prefix(token.size());
    return token;
}

char Lowercase(unsigned char character) {
    return static_cast<char>(std::tolower(character));
}

bool IsKeyword(std::string_view token, std::string_view keyword) {
    return std::equal(token.begin(), token.end(), keyword.begin(), keyword.end(),
                      [](unsigned char actual, char expected) { return Lowercase(actual) == expected; });
}

bool IsIdentifierStart(char character) {
    return (character >= 'a' && character <= 'z') ||
           (character >= 'A' && character <= 'Z') || character == '_';
}

bool IsIdentifier(std::string_view name) {
    return !name.empty() && IsIdentifierStart(name.front()) &&
           std::all_of(name.begin() + 1, name.end(), [](char character) {
               return IsIdentifierStart(character) || (character >= '0' && character <= '9');
           });
}
}

SQLStatement SQLParser::ParseQuery(const std::string& query) {
    auto input = Trim(query);
    if (!input.empty() && input.back() == ';') input.remove_suffix(1);

    const auto command = TakeToken(input);
    SQLStatement statement;
    std::string_view table_name;
    if (IsKeyword(command, "select")) {
        if (TakeToken(input) != "*" || !IsKeyword(TakeToken(input), "from")) return {};
        table_name = TakeToken(input);
        if (!Trim(input).empty()) return {};
        statement.type = StatementType::SELECT;
    } else if (IsKeyword(command, "insert")) {
        if (!IsKeyword(TakeToken(input), "into")) return {};
        table_name = TakeToken(input);
        if (!IsKeyword(TakeToken(input), "values")) return {};
        auto value = Trim(input);
        if (value.empty()) return {};
        if (value.front() == '\'') {
            if (value.size() < 2 || value.back() != '\'') return {};
            value.remove_prefix(1);
            value.remove_suffix(1);
            if (value.find('\'') != std::string_view::npos) return {};
        } else if (value.find_first_of(" \t\r\n\f\v;'") != std::string_view::npos) {
            return {};
        }
        statement.record_data.assign(value);
        statement.type = StatementType::INSERT;
    } else {
        return {};
    }
    if (!IsIdentifier(table_name)) return {};
    statement.table_name.assign(table_name);
    std::transform(statement.table_name.begin(), statement.table_name.end(), statement.table_name.begin(),
                   Lowercase);
    return statement;
}
}
