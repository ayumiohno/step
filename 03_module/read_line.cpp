#pragma once
#include "node.hpp"
#include "read_token.hpp"
#include <assert.h>
#include <ctype.h>
#include <iostream>
#include <stack>
#include <string>

struct AdminTree {
public:
    AdminTree(Node* root)
        : root(std::move(root)) {}

    void startBracket()
    {
        brackets.push(prior_insert_point);
    }

    void endBracket()
    {
        assert(!brackets.empty());
        prior_insert_point = brackets.top();
    }

    Node* getNonPriorInsertPoint()
    {
        return non_prior_insert_point = brackets.empty() ? root : brackets.top();
    }

    Node* getPriorInsertPoint() const
    {
        return prior_insert_point;
    }

    void setPriorInsertPoint(Node* node)
    {
        prior_insert_point = node;
    }

private:
    Node* root;
    Node* prior_insert_point;
    Node* non_prior_insert_point;
    std::stack<Node*> brackets;
};

void insertNode(Token token, AdminTree& admin_tree)
{
    Node* insert_point;
    if (token.isFunc() && !token.isPrior()) {
        insert_point = admin_tree.getNonPriorInsertPoint();
    } else if (token.isNumber() || token.isFunc()) {
        insert_point = admin_tree.getPriorInsertPoint();
    }
    Node* node = new Node{std::move(token)};
    insert_point->insert(node);
    if (token.isFunc() && token.isPrior()) {
        admin_tree.setPriorInsertPoint(node);
    }
}

void readLine(const std::string& line, AdminTree& admin_tree)
{
    int index = 0;
    while (index < line.size()) {
        if (isdigit(line.at(index))) {
            insertNode(std::move(readNumber(line, index)), admin_tree);
        } else if (line.at(index) == '+') {
            insertNode(std::move(readPlus(line, index)), admin_tree);
        } else if (line.at(index) == '-') {
            insertNode(std::move(readMinus(line, index)), admin_tree);
        } else if (line.at(index) == '*') {
            insertNode(std::move(readTimes(line, index)), admin_tree);
        } else if (line.at(index) == '/') {
            insertNode(std::move(readDividedBy(line, index)), admin_tree);
        } else if (line.at(index) == '(') {
            admin_tree.startBracket();
        } else if (line.at(index) == ')') {
            admin_tree.endBracket();
        } else {
            std::cout << "Invalid character: " << line.at(index) << std::endl;
        }
    }
}
