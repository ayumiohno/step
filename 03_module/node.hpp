#pragma once
#include "def_token.hpp"
#include <assert.h>
#include <iostream>
#include <stack>
#include <string>

struct Node {

    Node(Token token) : token(std::move(token)) {}
    ~Node() {}

    void setParent(Node* object) { this->parent = object; }
    void setLeft(Node* object) { this->left = object; }
    void setRight(Node* object) { this->right = object; }

    void insert(Node* object)
    {
        if (this->right != nullptr) {
            object->setLeft(this->right);
            this->right->setParent(object);
        }
        this->setRight(object);
        object->setParent(this);
    }

    double getValue()
    {
        if (token.isNumber()) {
            auto value = token.getValue();
            std::cerr << "NUMBER: " << value << std::endl;
            return value;
        } else if (token.isRoot()) {
            assert(right != nullptr);
            auto value = token.getAnswer(right->getValue());
            delete right;
            std::cerr << "ROOT: " << value << std::endl;
            return value;
        } else {
            assert(left != nullptr);
            assert(right != nullptr);
            auto left_value = left->getValue();
            delete left;
            auto right_value = right->getValue();
            delete right;
            auto value = token.calcurate(left_value, right_value);
            std::cerr << "OPERATOR: " << value << std::endl;
            return value;
        }
    }

private:
    Node* parent;
    Node* left;
    Node* right;
    Token token;
};

struct AdminTree {
public:
    AdminTree(Node* root)
        : root(std::move(root))
    {
        this->prior_insert_point = this->root;
        this->non_prior_insert_point = this->root;
    }

    void init()
    {
        this->prior_insert_point = this->root;
        this->non_prior_insert_point = this->root;
    }

    void startBracket()
    {
        brackets.push(prior_insert_point);
    }

    void endBracket()
    {
        assert(!brackets.empty());
        prior_insert_point = brackets.top();
        brackets.pop();
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
