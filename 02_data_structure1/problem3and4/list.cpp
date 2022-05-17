#include "list.hpp"
#include "node.hpp"
#include <assert.h>
#include <iostream>
#include <memory>
#include <string>

void List::addLast(std::shared_ptr<Node> node)
{
    //nodeのleftに現在のlastを設定
    assert(node != nullptr);
    node->setLeft(this->last, tag);
    if (this->last != nullptr) {
        //現在のlastのleftにnodeを設定
        this->last->setRight(node, tag);
    }
    if (this->first == nullptr) {
        //firstにnodeを設定(firstがなかった場合)
        this->setFirst(node);
    }
    //nodeをlastに設定
    this->setLast(node);
}

void List::deleteFirst()
{
    assert(this->first != nullptr);
    this->first->deleteNode();
}

std::shared_ptr<Node> List::find(const std::string& url)
{
    if (this->first == nullptr) {
        //listが空だった場合
        return this->first;
    }
    return this->first->findLoop(url, tag);
}
