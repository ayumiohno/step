#pragma once
#include "data.hpp"

/******************************************************
ほしい機能(改良版)
* listの最後に追加
* 探索
* Node自身を削除
* listの末尾に移動 
*******************************************************/

struct Node;

enum class Tag {
    HASH,
    ORDER
};

//list1つにつき1つ
//hashのarrayの要素に一つずつ・入った順番管理listで一つ
struct List {
public:
    List(const Tag& tag)
        : tag(tag)
    {
        this->first = nullptr;
        this->last = nullptr;
    }

    ~List() {}

    void setFirst(Node* node)
    {
        this->first = node;
    }

    void setLast(Node* node)
    {
        this->last = node;
    }

    void addLast(Node* node);

    void deleteFirst();

    Node* find(Data* object);

protected:
    Node* first;
    Node* last;
    Tag tag;
};
