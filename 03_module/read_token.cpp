#include "def_token.hpp"
#include "read_token.hpp"
#include <assert.h>
#include <functional>
#include <string>

bool isDigit(const char& c) { return int(c) >= int('0') && int(c) - int('0') <= 9; }

Token* readNumber(const std::string& line, int& index)
{
    double number = 0.0;
    while (index < line.size() && isDigit(line.at(index))) {
        number = number * 10 + (int(line.at(index)) - int('0'));
        ++index;
    }
    double decimal = 0.1;
    if (index < line.size() && line.at(index) == '.') {
        ++index;
        while (index < line.size() && isDigit(line.at(index))) {
            number += (int(line.at(index)) - int('0')) * decimal;
            decimal *= 0.1;
            ++index;
        }
    }
    Token* token = new Token{Tag::NUMBER, number};
    std::cerr << number << std::endl;
    return token;
}

Token* readPlus(const std::string& line, int& index)
{
    ++index;
    Token* token = new Token{Tag::NON_PRIOR,
        [](const auto& a, const auto& b) { return a + b; }};
    return token;
}

Token* readMinus(const std::string& line, int& index)
{
    ++index;
    Token* token = new Token{Tag::NON_PRIOR,
        [](const auto& a, const auto& b) { return a - b; }};
    return token;
}

Token* readTimes(const std::string& line, int& index)
{
    ++index;
    Token* token = new Token{Tag::PRIOR,
        [](const auto& a, const auto& b) { return a * b; }};
    return token;
}

Token* readDividedBy(const std::string& line, int& index)
{
    ++index;
    Token* token = new Token{Tag::PRIOR,
        [](const auto& a, const auto& b) { return a / b; }};
    return token;
}
