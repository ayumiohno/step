#include "def_token.hpp"
#include "node.hpp"
#include "read_line.hpp"
#include <iostream>
#include <string>

int main()
{
    Token root_token{Tag::ROOT};
    Node* root = new Node{std::move(root_token), 0};

    AdminTree admin_tree{root};
    double answer;

    while (true) {
        std::string formula;
        std::cin >> formula;
        if (formula == "exit") {
            break;
        }
        readLine(formula, admin_tree);
        answer = root->getValue();
        std::cout << formula << " = " << answer << std::endl;
        admin_tree.init();
    }
    delete root;
    /*
    readLine("0+2", admin_tree);
    answer = root->getValue();
    std::cout << "answer: " << answer << std::endl;
    admin_tree.init();

    readLine("4.3+(1.3+2.5)*((1.23+2.21)*3.45+1.4*2.5)", admin_tree);
    answer = root->getValue();
    std::cout << "answer: " << answer << std::endl;
    admin_tree.init();
    */

    return 0;
}
