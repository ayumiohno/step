#include<iostream>
#include<vector>
#include"read_line.hpp"

int main()
{
    while (true) {
        std::string formula;
        std::cin >> formula;
        if (formula == "exit") {
            break;
        }
        std::cout << formula;

        std::vector<Token> tokens;
        int index = 0;

        readLine(formula, tokens);

        std::cout << " = " <<  evaluatePlMi(tokens, index) << std::endl;
    }

    return 0;
}