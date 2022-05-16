#pragma once
#include "list.cpp"
#include "node.hpp"

/******************************************************
ほしい機能(改良版)
* listの最後に追加
* 探索
* Node自身を削除
* listの末尾に移動 
*******************************************************/

void Node::Graph::leaveList()
{
    this->before->hash.next = this->next;
    if (this->next == nullptr) {
        this->list->shiftLast();
    }
    this->next->hash.before = this->before;
    if (this->before == nullptr) {
        this->list->shiftFirst();
    }
}

Node* Node::findLoopForHash(Data* object)
{
    if (!hash.next) {
        return nullptr;
    } else {
        if (this->element == object) {
            return this;
        } else {
            hash.next->findLoopForHash(object);
        }
    }
}

auto Node::moveToLastForOrder()
{
    order.leaveList();
    order.list->addLast(this);
}

auto Node::leaveList()
{
    hash.leaveList();
    order.leaveList();
}
