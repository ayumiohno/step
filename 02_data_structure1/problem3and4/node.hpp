#pragma once
#include "list.hpp"
#include <iostream>
#include <memory>

//Element一つにつき1個
struct Node : public std::enable_shared_from_this<Node> {

    struct Unit {
        std::weak_ptr<Node> left;
        std::weak_ptr<Node> right;
        std::shared_ptr<List> list;
    } hash, order;

    ~Node() {}

    Node(std::shared_ptr<Data> element, std::shared_ptr<List> hash_list, std::shared_ptr<List> order_list)
        : element(std::move(element))
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

    std::weak_ptr<Node> findLoop(std::string url, const Tag& tag)
    {
        if (this->element->getUrl() == url) {
            return shared_from_this();
        } else {
            auto right_ptr = refUnit(tag).right.lock();
            if (right_ptr == nullptr) {
                return refUnit(tag).right;
            } else {
                right_ptr->findLoop(url, tag);
            }
        }
    }

    void setLeft(std::weak_ptr<Node> node, const Tag& tag)
    {
        std::cout << "called setLeft " << std::endl;
        refUnit(tag).left = node;
        std::cout << "done setLeft " << std::endl;
    }

    void setRight(std::weak_ptr<Node> node, const Tag& tag)
    {
        refUnit(tag).right = node;
    }

    void leaveList(const Tag& tag)
    {
        auto left_ptr = refUnit(tag).left.lock();
        if (left_ptr == nullptr) {
            refUnit(tag).list->setFirst(refUnit(tag).right);
        } else {
            left_ptr->setRight(refUnit(tag).right, tag);
        }
        auto right_ptr = refUnit(tag).right.lock();
        if (right_ptr == nullptr) {
            refUnit(tag).list->setLast(refUnit(tag).left);
        } else {
            right_ptr->setLeft(refUnit(tag).left, tag);
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
        std::cout << "called setLeft " << std::endl;
    }

private:
    std::shared_ptr<Data> element;
};
