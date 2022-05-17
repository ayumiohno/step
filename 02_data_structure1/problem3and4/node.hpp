#pragma once
#include "list.hpp"
#include <iostream>
#include <memory>

//Element一つにつき1個
struct Node : public std::enable_shared_from_this<Node> {

    struct Unit {
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
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
        } else {
            return order;
        }
    }

    std::shared_ptr<Node> findLoop(std::string url, const Tag& tag)
    {
        //std::cout << this->element->getUrl() << " " << url << std::endl;
        if (this->element->getUrl() == url) {
            //std::cout << "correct! " << std::endl;
            return shared_from_this();
        } else {
            auto right_ptr = refUnit(tag).right;
            if (right_ptr == nullptr) {
                //std::cout << "no right " << std::endl;
                return refUnit(tag).right;
            } else {
                //std::cout << "next right " << std::endl;
                right_ptr->findLoop(url, tag);
            }
        }
    }

    void setLeft(std::shared_ptr<Node> node, const Tag& tag)
    {
        //std::cout << "called setLeft " << std::endl;
        refUnit(tag).left = node;
        //std::cout << "done setLeft " << std::endl;
    }

    void setRight(std::shared_ptr<Node> node, const Tag& tag)
    {
        refUnit(tag).right = node;
    }

    void leaveList(const Tag& tag)
    {
        auto left_ptr = refUnit(tag).left;
        if (left_ptr == nullptr) {
            refUnit(tag).list->setFirst(refUnit(tag).right);
        } else {
            left_ptr->setRight(refUnit(tag).right, tag);
        }
        auto right_ptr = refUnit(tag).right;
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
        //delete this;
        //std::cout << "called setLeft " << std::endl;
    }

    void showContent()
    {
        std::cout << "result " << this->element->showContent() << std::endl;
    }

private:
    std::shared_ptr<Data> element;
};
