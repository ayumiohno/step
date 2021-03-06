#pragma once
#include "def_token.hpp"
#include <assert.h>
#include <iostream>
#include <stack>
#include <string>

struct Node {

    Node(const Token& token)
        : token(std::move(token))
    {
        this->right = nullptr;
        this->left = nullptr;
        this->parent = nullptr;
    }
    ~Node() { std::cerr << "node deleted" << std::endl; }

    void setParent(Node* object) { this->parent = object; }
    void setLeft(Node* object) { this->left = object; }
    void setRight(Node* object) { this->right = object; }

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

    bool isNodeOfFunc() const { return this->token.isFunc(); };
    bool isNodeOfPrior() const { return this->token.isPrior(); };

    double getValue()
    {
        if (token.isNumber()) {
            auto value = token.getValue();
            std::cerr << "NUMBER: " << value << std::endl;
            return value;
        } else if (token.isRoot()) {
            assert(right != nullptr);
            auto value = token.getAnswer(right->getValue());
            delete this->right;
            this->right = nullptr;
            std::cerr << "ROOT: " << value << std::endl;
            return value;
        } else {
            assert(left != nullptr);
            assert(right != nullptr);
            auto left_value = left->getValue();
            delete this->left;
            this->left = nullptr;
            auto right_value = right->getValue();
            delete this->right;
            this->right = nullptr;
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
        this->node_before = this->root;
    }
    ~AdminTree() { std::cerr << "admin_tree deleted" << std::endl; }

    void init()
    {
        assert(brackets.empty());
        this->prior_insert_point = this->root;
        this->non_prior_insert_point = this->root;
        this->node_before = this->root;
    }

    void startBracket()
    {
        brackets.push({node_before, prior_insert_point});
        this->prior_insert_point = this->node_before;
    }

    void endBracket()
    {
        assert(!brackets.empty());
        prior_insert_point = brackets.top().second;
        brackets.pop();
    }

    Node* getNonPriorInsertPoint()
    {
        non_prior_insert_point = brackets.empty() ? root : brackets.top().first;
        return non_prior_insert_point;
    }

    Node* getPriorInsertPoint() const
    {
        return prior_insert_point;
    }

    Node* getNodeBefore() const
    {
        return node_before;
    }

    void setPriorInsertPoint(Node* node)
    {
        prior_insert_point = std::move(node);
    }

    void setNodeBefore(Node* node)
    {
        node_before = std::move(node);
    }

private:
    Node* root;
    Node* prior_insert_point;
    Node* non_prior_insert_point;
    Node* node_before;
    std::stack<std::pair<Node*, Node*>> brackets;
};
