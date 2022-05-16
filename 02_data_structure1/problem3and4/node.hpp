#pragma once
#include "data.cpp"

struct List;

//Element一つにつき1個
struct Node {
    Node(Data* element)
        : element(std::move(element)) {}

    struct Graph {

        Node* next;
        Node* before;
        List* list;

        void leaveList();
    };

    Graph hash, order;

    Data* element;

    Node* findLoopForHash(Data* object);

    auto moveToLastForOrder();
    auto leaveList();
};
