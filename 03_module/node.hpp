#pragma once
#include "def_token.hpp"
#include <assert.h>
#include <iostream>
#include <stack>
#include <string>

struct Node {

    Node(Token token, int ref_num)
        : token(std::move(token)), ref_num(std::move(ref_num)) {}
    ~Node()
    {
        std::cerr << "deleted: " << this->ref_num << std::endl;
    }

    void setParent(Node* object) { this->parent = object; }
    void setLeft(Node* object) { this->left = object; }
    void setRight(Node* object) { this->right = object; }

    void insert(Node* object)
    {
        if (this->right != nullptr) {
            object->setLeft(this->right);
            this->right->setParent(object);
            std::cout << "parent: " << object->getRefNum() << " right: " << this->right->getRefNum() << std::endl;
        }
        this->setRight(object);
        object->setParent(this);
        std::cout << "parent: " << this->getRefNum() << " right: " << object->getRefNum() << std::endl;
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
            this->right = nullptr;
            std::cerr << "ROOT: " << value << std::endl;
            return value;
        } else {
            assert(left != nullptr);
            assert(right != nullptr);
            auto left_value = left->getValue();
            delete left;
            this->left = nullptr;
            auto right_value = right->getValue();
            delete right;
            this->right = nullptr;
            auto value = token.calcurate(left_value, right_value);
            std::cerr << "OPERATOR: " << value << std::endl;
            return value;
        }
    }

    int getRefNum() { return this->ref_num; }

private:
    Node* parent;
    Node* left;
    Node* right;
    Token token;
    int ref_num;
};

struct AdminTree {
public:
    AdminTree(Node* root)
        : root(std::move(root))
    {
        this->prior_insert_point = this->root;
        this->non_prior_insert_point = this->root;
    }
    ~AdminTree() { std::cout << "admin deleted" << std::endl; }

    void init()
    {
        assert(brackets.empty());
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
