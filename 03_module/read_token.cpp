#pragma once
#include <assert.h>
#include <functional>
#include <iostream>
#include <string>

enum class Tag {
    PRIOR,
    NON_PRIOR,
    NUMBER,
    ROOT
};

struct Token {

public:
    Token(Tag tag, std::function<double(double, double)> func)
        : tag(std::move(tag)), func(std::move(func))
    {
        assert(tag == Tag::PRIOR || tag == Tag::NON_PRIOR);
    }

    Token(const Tag& tag, double value)
        : tag(std::move(tag)), value(std::move(value))
    {
        assert(tag == Tag::NUMBER);
    }

    Token(const Tag& tag)
        : tag(std::move(tag))
    {
        assert(tag == Tag::ROOT);
    }


    bool isNumber() const { return tag == Tag::NUMBER; }
    bool isFunc() const { return tag == Tag::PRIOR || tag == Tag::NON_PRIOR; }
    bool isRoot() const { return tag == Tag::ROOT; }

    bool isPrior() const
    {
        assert(isFunc());
        return tag == Tag::PRIOR;
    }

    double getValue() const
    {
        assert(isNumber());
        return value;
    }

    double calcurate(const double& a, const double& b) const
    {
        assert(isFunc());
        return func(a, b);
    }

    double getAnswer(const double& b) const
    {
        assert(isRoot());
        return b;
    }

private:
    Tag tag;
    double value;
    std::function<double(double, double)> func;
};

Token readNumber(const std::string& line, int& index)
{
    double number = 0;
    while (index < line.size() && line.at(index)) {
        number = number * 10 + int(line.at(index));
        ++index;
    }
    double decimal = 0.1;
    if (index < line.size() && line.at(index) == '.') {
        while (index < line.size() && line.at(index)) {
            number += int(line.at(index)) * decimal;
            decimal /= 10;
            ++index;
        }
    }
    Token token{Tag::NUMBER, number};
    return token;
}

Token readPlus(const std::string& line, int& index)
{
    ++index;
    Token token{Tag::NON_PRIOR,
        [](const auto& a, const auto& b) { return a + b; }};
    return token;
}

Token readMinus(const std::string& line, int& index)
{
    ++index;
    ++index;
    Token token{Tag::NON_PRIOR,
        [](const auto& a, const auto& b) { return a - b; }};
    return token;
}

Token readTimes(const std::string& line, int& index)
{
    ++index;
    Token token{Tag::PRIOR,
        [](const auto& a, const auto& b) { return a * b; }};
    return token;
}

Token readDividedBy(const std::string& line, int& index)
{
    ++index;
    Token token{Tag::PRIOR,
        [](const auto& a, const auto& b) { return a / b; }};
    return token;
}
