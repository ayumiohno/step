#pragma once
#include "data.hpp"
#include "list.hpp"
#include "node.hpp"
#include <array>
#include <iostream>
#include <memory>

//hase table の子クラスdehanaku 直接作る
//template <int Num>
struct Cache {

    Cache()
    {
        //this->hash_table = std::array<List, Num>{};
        for (int i = 0; i < 23; ++i) {
            this->hash_table.at(i) = std::make_shared<List>(Tag::HASH);
        }
        this->order_list = std::make_shared<List>(Tag::ORDER);
    }

    ~Cache() {}

    //どのarrayを参照するか探す(Listを返す)!
    int getHash(std::string url)
    {
        //auto url = data.refUrl();
        int score = 1;
        for (auto c : url) {
            score = score * (1 + scores.at(std::abs((int)(c - 'a') % 9))) % 23;
            std::cout << "checked: " << c << std::endl;
        }
        return score - 1;
    }

    void processNewData(std::string url, int hash_value)
    {
        ++data_count;
        std::cout << "called processNew data" << std::endl;
        auto hash_list = hash_table.at(hash_value);
        std::cout << "get hash_list" << std::endl;
        auto content = "aaa";  //loadContent(url);
        auto new_node = std::make_shared<Node>(std::make_shared<Data>(url, content), hash_list, order_list);
        hash_list->addLast(new_node);
        std::cout << "add last" << std::endl;
        if (data_count >= 30) {
            order_list->deleteFirst();
        }
        std::cout << "delete first" << std::endl;
        order_list->addLast(std::move(new_node));
    }

    void processData(std::string url)
    {
        int hash_value = getHash(url);
        std::cout << "done getHash. hash_value: " << hash_value << std::endl;
        /*        if (!hash_table.at(hash_value)) {
            List hash_list{Tag::HASH};
            hash_table.at(hash_value) = std::move(hash_list);
            processNewData(data, hash_value);
        } else {*/
        auto node_ptr = hash_table.at(hash_value)->find(url).lock();
        std::cout << "done node_ptr: " << node_ptr << std::endl;
        if (node_ptr == nullptr) {
            std::cout << "null" << std::endl;
            processNewData(url, hash_value);
        } else {
            std::cout << "exist" << std::endl;
            node_ptr->goToLast(Tag::ORDER);
        }
        //}
    }

    std::array<std::shared_ptr<List>, 23> hash_table;
    std::shared_ptr<List> order_list;
    const std::array<int, 9> scores = {2, 3, 5, 7, 11, 13, 17, 19, 23};
    int data_count = 0;
};
