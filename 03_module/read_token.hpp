#pragma once
#include "def_token.hpp"
#include <string>

bool isDigit(const char& c);

Token* readNumber(const std::string& line, int& index);

Token* readPlus(const std::string& line, int& index);

Token* readMinus(const std::string& line, int& index);

Token* readTimes(const std::string& line, int& index);

Token* readDividedBy(const std::string& line, int& index);
