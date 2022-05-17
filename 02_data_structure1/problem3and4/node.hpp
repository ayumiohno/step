#pragma once
#include "list.hpp"
#include <iostream>
#include <memory>
#include <string>

struct Node : public std::enable_shared_from_this<Node> {

    struct Unit {
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        std::shared_ptr<List> list;
    } hash, order;

    Node(std::shared_ptr<Data> element, std::shared_ptr<List> hash_list, std::shared_ptr<List> order_list)
        : element(std::move(element))
    {
        this->hash.list = std::move(hash_list);
        this->order.list = std::move(order_list);
    }

    ~Node() {}

    Unit& refUnit(const Tag& tag)
    {
        if (tag == Tag::HASH) {
            return hash;
        } else {
            return order;
        }
    }

    std::shared_ptr<Node> findLoop(const std::string& url, const Tag& tag)
    {
        if (this->element->getUrl() == url) {
            return shared_from_this();
        } else {
            auto right_ptr = refUnit(tag).right;
            if (right_ptr == nullptr) {
                return refUnit(tag).right;
            } else {
                right_ptr->findLoop(url, tag);
            }
        }
    }

    void setLeft(std::shared_ptr<Node> node, const Tag& tag)
    {
        refUnit(tag).left = node;
    }

    void setRight(std::shared_ptr<Node> node, const Tag& tag)
    {
        refUnit(tag).right = node;
    }

    void leaveList(const Tag& tag)
    {
        if (refUnit(tag).left == nullptr) {
            refUnit(tag).list->setFirst(refUnit(tag).right);
        } else {
            refUnit(tag).left->setRight(refUnit(tag).right, tag);
        }
        if (refUnit(tag).right == nullptr) {
            refUnit(tag).list->setLast(refUnit(tag).left);
        } else {
            refUnit(tag).right->setLeft(refUnit(tag).left, tag);
        }
    }

    void goToLast(const Tag& tag)
    {
        leaveList(tag);
        refUnit(tag).list->addLast(shared_from_this());
    }

    void deleteNode()
    {
        leaveList(Tag::HASH);
        leaveList(Tag::ORDER);
    }

    void showContent() const
    {
        std::cout << "!!!REASULT!!! " << this->element->showContent() << std::endl;
    }

private:
    std::shared_ptr<Data> element;
};
