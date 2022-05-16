#pragma once
#include "node.hpp"

/******************************************************
ほしい機能(改良版)
* listの最後に追加
* 探索
* Node自身を削除
* listの末尾に移動 
*******************************************************/


//list1つにつき1つ
//hashのarrayの要素に一つずつ・入った順番管理listで一つ
struct List {
public:
    List(Node* first, Node* last)
        : first(first), last(last) {}

    virtual void addLast(Node* node) = 0;
    virtual void shiftFirst() = 0;
    virtual void shiftLast() = 0;

protected:
    Node* first;
    Node* last;
};

struct Hash : public List {

    using List::List;

    void addLast(Node* node) override
    {
        node->hash.before = this->last;
        this->last->hash.next = node;
        this->last = node;
    }
    void shiftFirst() override
    {
        this->first = this->first->hash.next;
    }
    void shiftLast() override
    {
        this->last = this->last->hash.before;
    }
    Node* find(Data* object)
    {
        if (!this->first) {
            return nullptr;
        }
        this->first->findLoopForHash(object);
    }
};

struct Order : public List {

    using List::List;

    void addLast(Node* node) override
    {
        node->order.before = this->last;
        this->last->order.next = node;
        this->last = node;
    }
    void shiftFirst() override
    {
        this->first = this->first->order.next;
    }
    void shiftLast() override
    {
        this->last = this->last->order.before;
    }
};
