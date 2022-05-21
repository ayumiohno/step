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

    Token(Tag tag, double value)
        : tag(std::move(tag)), value(std::move(value))
    {
        assert(tag == Tag::NUMBER);
    }

    Token(Tag tag)
        : tag(std::move(tag))
    {
        assert(tag == Tag::ROOT);
    }

    ~Token() { std::cerr << "token deleted" << std::endl; }

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
