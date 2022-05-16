#pragma once
#include "data.hpp"
#include "list.hpp"
#include "node.hpp"
#include <array>
#include <iostream>

//hase table の子クラスdehanaku 直接作る
//template <int Num>
struct Cache {

    Cache()
    {
        //this->hash_table = std::array<List, Num>{};
        std::array<List*, 23> hash_table;
        for (int i = 0; i < 23; ++i) {
            List hash_list{Tag::HASH};
            hash_table.at(i) = &hash_list;
        }
        this->hash_table = std::move(hash_table);
        List order_list{Tag::ORDER};
        this->order_list = &order_list;
    }

    ~Cache() {}

    //どのarrayを参照するか探す(Listを返す)!
    int getHash(Data& data)
    {
        auto url = data.refUrl();
        int score = 1;
        for (auto c : url) {
            score = score * (1 + scores.at(std::abs((int)(c - 'a') % 9))) % 23;
            std::cout << "checked: " << c << std::endl;
        }
        return score - 1;
    }

    void processNewData(Data* data, int hash_value)
    {
        std::cout << "called processNew data" << std::endl;
        auto hash_list = hash_table.at(hash_value);
        std::cout << "get hash_list" << std::endl;
        Node new_node{data, hash_list, order_list};
        std::cout << "new_node" << std::endl;
        hash_list->addLast(&new_node);  //TODO
        std::cout << "add last" << std::endl;
        order_list->deleteFirst();  //TODO
        std::cout << "delete first" << std::endl;
        order_list->addLast(&new_node);
    }

    void processData(Data* data)
    {
        int hash_value = getHash(*data);
        std::cout << "done getHash. hash_value: " << hash_value << std::endl;
        /*        if (!hash_table.at(hash_value)) {
            List hash_list{Tag::HASH};
            hash_table.at(hash_value) = std::move(hash_list);
            processNewData(data, hash_value);
        } else {*/
        auto node_ptr = hash_table.at(hash_value)->find(data);  //TODO
        std::cout << "done node_ptr" << std::endl;
        if (node_ptr == nullptr) {
            std::cout << "null" << std::endl;
            delete node_ptr;
            std::cout << "null" << std::endl;
            processNewData(data, hash_value);
        } else {
            std::cout << "exist" << std::endl;
            node_ptr->goToLast(Tag::ORDER);
        }
        //}
    }

    std::array<List*, 23> hash_table;
    List* order_list;
    const std::array<int, 9> scores = {2, 3, 5, 7, 11, 13, 17, 19, 23};
};
