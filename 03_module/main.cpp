#include "def_token.hpp"
#include "node.hpp"
#include "read_line.hpp"
#include <iostream>
#include <string>

int main()
{
    Token* root_token = new Token{Tag::ROOT};
    Node* root = new Node{(root_token)};
    AdminTree admin_tree{root};

    while (true) {
        std::string formula;
        std::cin >> formula;
        if (formula == "exit") {
            break;
        }
        readLine(formula, admin_tree);
        double answer = root->getValue();
        std::cout << formula << " = " << answer << std::endl;
        admin_tree.init();
    }
    delete root_token;
    delete root;

    return 0;
}
