#pragma once
#include <assert.h>
#include <functional>
#include <iostream>
#include <string>

enum class Tag {
    PL_MI,
    TM_DV,
    NUM,
    BR_B,
    BR_E
};

struct Token{
    
public:
    Token(Tag tag, std::function<double(double, double)> func)
        : tag(std::move(tag)), func(std::move(func))
    {
        assert(tag == Tag::PL_MI || tag == Tag::TM_DV);
    }

    Token(Tag tag, double value)
        : tag(std::move(tag)), value(std::move(value))
    {
        assert(tag == Tag::NUM);
    }

    Token(Tag tag)
        : tag(std::move(tag))
    {
        assert(tag == Tag::BR_B || tag == Tag::BR_E);
    }

    Tag tag;
    double value;
    std::function<double(double, double)> func;
};