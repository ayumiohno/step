#include "def_token.hpp"
#include "node.hpp"
#include "read_line.hpp"
#include <iostream>
#include <string>

int main()
{
    Node* root = new Node{Token{Tag::ROOT}};
    AdminTree admin_tree{root};

    while (true) {
        std::string formula;
        std::cin >> formula;
        if (formula == "exit") {
            break;
        }
        std::cout << formula;
        formatLine(formula);
        readLine(formula, admin_tree);
        double answer = root->getValue();
        std::cout << " = " << answer << std::endl;
        admin_tree.init();
    }
    delete root;

    return 0;
}
