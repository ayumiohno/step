#pragma once
#include "def_token.hpp"
#include <assert.h>
#include <iostream>
#include <stack>
#include <string>

struct Node {

    Node(Token* token)
        : token(std::move(token))
    {
        assert(token != nullptr);
        this->right = nullptr;
        this->left = nullptr;
        this->parent = nullptr;
    }
    ~Node() { std::cerr << "node deleted" << std::endl; }

    void setParent(Node* object) { this->parent = object; }
    void setLeft(Node* object) { this->left = object; }
    void setRight(Node* object) { this->right = object; }

    void deleteSelf()
    {
        delete this->token;
        delete this;
    }

    void insert(Node* object)
    {
        assert(object != nullptr);
        if (this->right != nullptr) {
            object->setLeft(this->right);
            this->right->setParent(object);
        }
        this->setRight(object);
        object->setParent(this);
    }

    double getValue()
    {
        if (token->isNumber()) {
            auto value = token->getValue();
            std::cerr << "NUMBER: " << value << std::endl;
            return value;
        } else if (token->isRoot()) {
            assert(right != nullptr);
            auto value = token->getAnswer(right->getValue());
            right->deleteSelf();
            this->right = nullptr;
            std::cerr << "ROOT: " << value << std::endl;
            return value;
        } else {
            assert(left != nullptr);
            assert(right != nullptr);
            auto left_value = left->getValue();
            left->deleteSelf();
            this->left = nullptr;
            auto right_value = right->getValue();
            right->deleteSelf();
            this->right = nullptr;
            auto value = token->calcurate(left_value, right_value);
            std::cerr << "OPERATOR: " << value << std::endl;
            return value;
        }
    }

    int getRef()
    {
        return ref_num;
    }
    void setRef(int num)
    {
        this->ref_num = num;
    }


private:
    int ref_num;
    Node* parent;
    Node* left;
    Node* right;
    Token* token;
};

struct AdminTree {
public:
    AdminTree(Node* root)
        : root(std::move(root))
    {
        this->prior_insert_point = this->root;
        this->non_prior_insert_point = this->root;
    }
    ~AdminTree() { std::cerr << "admin_tree deleted" << std::endl; }

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
        non_prior_insert_point = brackets.empty() ? root : brackets.top();
        return non_prior_insert_point;
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
