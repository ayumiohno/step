#pragma once
#include "list.hpp"
#include <iostream>

//Element一つにつき1個
struct Node {

    struct Unit {
        Node* left;
        Node* right;
        List* list;
    } hash, order;

    ~Node() {}

    Node(Data* element, List* hash_list, List* order_list)
        : element(element)
    {
        hash.list = hash_list;
        order.list = order_list;
    }

    Unit& refUnit(const Tag& tag)
    {
        if (tag == Tag::HASH) {
            return hash;
        } else if (tag == Tag::ORDER) {
            return order;
        }
    }

    Node* findLoop(Data* object, const Tag& tag)
    {
        if (!refUnit(tag).right) {
            return nullptr;
        } else {
            if (this->element == object) {
                return this;
            } else {
                refUnit(tag).left->findLoop(object, tag);
            }
        }
    }

    void setLeft(Node* node, const Tag& tag)
    {
        std::cout << "called setLeft " << std::endl;
        refUnit(tag).left = node ? node : nullptr;
        std::cout << "done setLeft " << std::endl;
    }

    void setRight(Node* node, const Tag& tag)
    {
        refUnit(tag).right = node ? node : nullptr;
    }

    void leaveList(const Tag& tag)
    {
        refUnit(tag).left->setRight(refUnit(tag).right, tag);
        if (!refUnit(tag).right) {
            refUnit(tag).list->setLast(refUnit(tag).left);
        }
        refUnit(tag).right->setLeft(refUnit(tag).left, tag);
        if (!refUnit(tag).left) {
            refUnit(tag).list->setFirst(refUnit(tag).right);
        }
    }

    void goToLast(const Tag& tag)
    {
        leaveList(tag);
        refUnit(tag).list->addLast(this);
    }

    void deleteNode()
    {
        leaveList(Tag::HASH);
        leaveList(Tag::ORDER);
        delete this;
        std::cout << "called setLeft " << std::endl;
    }

private:
    Data* element;
};
