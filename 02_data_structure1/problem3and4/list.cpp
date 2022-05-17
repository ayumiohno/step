#include "list.hpp"
#include "node.hpp"
#include <assert.h>
#include <iostream>
#include <memory>
#include <string>

void List::addLast(std::shared_ptr<Node> node)
{
    assert(node != nullptr);
    node->setLeft(this->last, tag);
    if (this->last != nullptr) {
        this->last->setRight(node, tag);
    }
    if (this->first == nullptr) {
        this->setFirst(node);
    }
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
        return this->first;
    }
    return this->first->findLoop(url, tag);
}
