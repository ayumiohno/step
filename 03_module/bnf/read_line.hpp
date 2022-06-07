#pragma once
#include"read_token.hpp"
#include <string>
#include <vector>

void readLine(const std::string& line, std::vector<Token>& tokens);

double evaluatePlMi(std::vector<Token>& tokens, int& index);
double evaluateTmDv(std::vector<Token>& tokens, int& index);
double evaluateBr(std::vector<Token>& tokens, int& index);
double evaluateNum(std::vector<Token>& tokens, int& index);