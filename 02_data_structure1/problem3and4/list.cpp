#include "list.hpp"
#include "node.hpp"
#include <assert.h>
#include <iostream>
#include <memory>

/******************************************************
ほしい機能(改良版)
* listの最後に追加
* 探索
* Node自身を削除
* listの末尾に移動 
*******************************************************/


void List::addLast(std::weak_ptr<Node> node)
{
    std::cout << "call add last" << std::endl;
    auto node_ptr = node.lock();
    assert(node_ptr != nullptr);
    node_ptr->setLeft(this->last, tag);
    std::cout << "set left" << std::endl;
    node_ptr->setLeft(this->last, tag);
    auto last_ptr = this->last.lock();
    if (last_ptr != nullptr) {
        last_ptr->setRight(node, tag);
    }
    if (this->first.lock() == nullptr) {
        this->first = node;
        std::cout << "new first" << std::endl;
    }
    this->last = node;
}

void List::deleteFirst()
{
    auto first_ptr = this->first.lock();  //TODO hidoi
    assert(first_ptr != nullptr);
    first_ptr->deleteNode();
}

std::weak_ptr<Node> List::find(std::string url)
{
    std::cout << "call find" << std::endl;
    auto first_ptr = this->first.lock();  //TODO hidoi
    if (first_ptr == nullptr) {
        std::cout << "nullptr " << std::endl;
        return this->first;
    }
    std::cout << "go to loop" << std::endl;
    first_ptr->findLoop(url, tag);
}
