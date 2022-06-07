#include "def_token.hpp"
#include "node.hpp"
#include "read_line.hpp"
#include "read_token.hpp"
#include <assert.h>
#include <iostream>
#include <string>


void insertNode(const Token& token, AdminTree& admin_tree)
{
    Node* insert_point;
    if (token.isFunc() && !token.isPrior()) {
        insert_point = admin_tree.getNonPriorInsertPoint();
    } else if (token.isFunc()) {
        insert_point = admin_tree.getPriorInsertPoint();
    } else if (token.isNumber()) {
        insert_point = admin_tree.getNodeBefore();
    }
    Node* node = new Node{token};
    assert(insert_point != nullptr);
    insert_point->insert(node);
    if (node->isNodeOfFunc()) {
        admin_tree.setNodeBefore(node);
        if (!node->isNodeOfPrior()) {
            admin_tree.setPriorInsertPoint(node);
        }
    }
}

void formatLine(std::string& line)
{
    int index = 0;
    if (line.at(0) == '+' || line.at(0) == '-') {
        line = "0" + line;
    }
    assert(isDigit(line.at(0)) || line.at(0) == '(');
}

void readLine(const std::string& line, AdminTree& admin_tree)
{
    int index = 0;
    while (index < line.size()) {
        if (isDigit(line.at(index))) {
            insertNode((readNumber(line, index)), admin_tree);
        } else if (line.at(index) == '+') {
            insertNode((readPlus(line, index)), admin_tree);
        } else if (line.at(index) == '-') {
            insertNode((readMinus(line, index)), admin_tree);
        } else if (line.at(index) == '*') {
            insertNode((readTimes(line, index)), admin_tree);
        } else if (line.at(index) == '/') {
            insertNode((readDividedBy(line, index)), admin_tree);
        } else if (line.at(index) == '(') {
            ++index;
            admin_tree.startBracket();
        } else if (line.at(index) == ')') {
            ++index;
            admin_tree.endBracket();
        } else {
            std::cout << "Invalid character: " << line.at(index) << std::endl;
        }
    }
}
