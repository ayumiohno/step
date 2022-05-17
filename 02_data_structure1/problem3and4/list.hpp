#pragma once
#include "data.hpp"
#include <assert.h>
#include <memory>
#include <string>

struct Node;

enum class Tag {
    HASH,
    ORDER
};

struct List {
public:
    List(const Tag& tag)
        : tag(tag) {}

    ~List() {}

    void setFirst(std::shared_ptr<Node> node)
    {
        this->first = node;
    }

    void setLast(std::shared_ptr<Node> node)
    {
        this->last = node;
    }

    void addLast(std::shared_ptr<Node> node);

    void deleteFirst();

    std::shared_ptr<Node> find(const std::string& url);

protected:
    std::shared_ptr<Node> first;
    std::shared_ptr<Node> last;
    Tag tag;
};
