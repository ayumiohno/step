#pragma once
#include "read_token.cpp"

struct Node {

    Node(Token token) : token(std::move(token)) {}
    ~Node() {}

    void deleteSelf() { delete this; }

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
            return token.getValue();
        } else if (token.isRoot()) {
            assert(right != nullptr);
            return token.getAnswer(right->getValue());
        } else {
            assert(left != nullptr);
            assert(right != nullptr);
            auto value = token.calcurate(left->getValue(), right->getValue());
            left->deleteSelf();  //TODO value返した後に消したい
            right->deleteSelf();
            return value;
        }
    }

private:
    Node* parent;
    Node* left;
    Node* right;
    Token token;
};
