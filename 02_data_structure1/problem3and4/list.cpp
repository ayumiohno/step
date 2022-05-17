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


void List::addLast(std::shared_ptr<Node> node)
{
    //std::cout << "call add last" << std::endl;
    auto node_ptr = node;
    assert(node_ptr != nullptr);
    node_ptr->setLeft(this->last, tag);
    //std::cout << "set left" << std::endl;
    auto last_ptr = this->last;
    if (last_ptr != nullptr) {
        last_ptr->setRight(node, tag);
    }
    if (this->first == nullptr) {
        this->setFirst(node);
        //std::cout << "new first" << std::endl;
    }
    this->setLast(node);
}

void List::deleteFirst()
{
    auto first_ptr = this->first;
    assert(first_ptr != nullptr);
    first_ptr->deleteNode();
}

std::shared_ptr<Node> List::find(std::string url)
{
    //std::cout << "call find" << std::endl;
    auto first_ptr = this->first;
    if (first_ptr == nullptr) {
        std::cout << "this list has no first" << std::endl;
        return this->first;
    }
    std::cout << "this list already has first. go to loop" << std::endl;
    return first_ptr->findLoop(url, tag);
}
