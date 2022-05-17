#pragma once
#include "data.hpp"
#include <assert.h>
#include <memory>

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
        //this->first = std::make_shared<Node>{nullptr};
        //this->last = std::make_shared<Node>{nullptr};
    }

    ~List() {}

    void setFirst(std::shared_ptr<Node> node)
    {
        this->first = node;
    }

    void setLast(std::shared_ptr<Node> node)
    {
        this->last = node;
    }

    /*std::weak_ptr<Node> getLast()
    {
        assert(this->last.lock() != nullptr);
        return this->last;
    }*/

    void addLast(std::shared_ptr<Node> node);

    void deleteFirst();

    std::shared_ptr<Node> find(std::string url);

protected:
    std::shared_ptr<Node> first;
    std::shared_ptr<Node> last;
    Tag tag;
};
