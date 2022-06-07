#include "read_line.hpp"

void readLine(const std::string& line, std::vector<Token>& tokens)
{
    int index = 0;
    while (index < line.size()) {
        if (isDigit(line.at(index))) {
            tokens.push_back(readNumber(line, index));
        } else if (line.at(index) == '+') {
            tokens.push_back(readPlus(line, index));
        } else if (line.at(index) == '-') {
            tokens.push_back(readMinus(line, index));
        } else if (line.at(index) == '*') {
            tokens.push_back(readTimes(line, index));
        } else if (line.at(index) == '/') {
            tokens.push_back(readDividedBy(line, index));
        } else if (line.at(index) == '(') {
            tokens.push_back(readBracketBegin(line, index));
        } else if (line.at(index) == ')') {
            tokens.push_back(readBracketEnd(line, index));
        } else {
            std::cout << "Invalid character: " << line.at(index) << std::endl;
        }
    }
}

double evaluatePlMi(std::vector<Token>& tokens, int& index)
{
    double value = evaluateTmDv(tokens, index);
    while(index < tokens.size() - 1 && tokens.at(index).tag == Tag::PL_MI){
        int opr_index = index;
        ++index;
        double r = evaluateTmDv(tokens, index);
        value = tokens.at(opr_index).func(value, r);
    }
    return value;
}

double evaluateTmDv(std::vector<Token>& tokens, int& index)
{
    double value = evaluateBr(tokens, index);
    while(index < tokens.size() - 1 && tokens.at(index).tag == Tag::TM_DV){
        int opr_index = index;
        ++index;
        double r = evaluateBr(tokens, index);
        value = tokens.at(opr_index).func(value, r);
    }
    return value;
}

double evaluateBr(std::vector<Token>& tokens, int& index)
{
    if (index < tokens.size() - 1  && tokens.at(index).tag == Tag::BR_B){
        ++index;
        double value = evaluatePlMi(tokens, index);
        if(tokens.at(index).tag == Tag::BR_E){
            ++index;
            return value;
        }
    } else {
        return evaluateNum(tokens, index);
    }
}

double evaluateNum(std::vector<Token>& tokens, int& index)
{
    ++index;
    return tokens.at(index - 1).value;
}
