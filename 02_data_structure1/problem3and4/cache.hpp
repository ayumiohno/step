#pragma once
#include "data.hpp"
#include "data_base.hpp"
#include "list.hpp"
#include "node.hpp"
#include <array>
#include <iostream>
#include <memory>
#include <string>

template <int TABLE_SIZE, int MEMORY_SIZE>
struct Cache {

    Cache()
    {
        for (int i = 0; i < TABLE_SIZE; ++i) {
            this->hash_table.at(i) = std::make_shared<List>(Tag::HASH);
        }
        this->order_list = std::make_shared<List>(Tag::ORDER);
        DataBase database{};
        this->database = std::move(database);
    }

    ~Cache() {}

    int getHash(const std::string& url) const
    {
        int score = 1;
        for (const auto& c : url) {
            score = score * (1 + scores.at(std::abs((int)(c - 'a') % 9))) % TABLE_SIZE;
        }
        std::cout << "url: " << url << " hash: " << score - 1 << std::endl;
        return score - 1;
    }

    void processNewData(const std::string& url, const int& hash_value)
    {
        if (data_count >= MEMORY_SIZE) {
            order_list->deleteFirst();
        } else {
            ++data_count;
        }
        auto content = database.loadContent(url);
        std::cout << "!!!REASULT!!! " << content << std::endl;
        auto& hash_list = hash_table.at(hash_value);
        auto new_node = std::make_shared<Node>(std::make_shared<Data>(url, content), hash_list, order_list);
        order_list->addLast(new_node);
        hash_list->addLast(new_node);
    }

    void processData(const std::string& url)
    {
        int hash_value = getHash(url);
        auto node = hash_table.at(hash_value)->find(url);
        if (node == nullptr) {
            std::cout << "create new data" << std::endl;
            processNewData(url, hash_value);
        } else {
            std::cout << "url already exist" << std::endl;
            node->showContent();
            node->goToLast(Tag::ORDER);
        }
    }

    std::array<std::shared_ptr<List>, TABLE_SIZE> hash_table;
    std::shared_ptr<List> order_list;
    DataBase database;
    const std::array<int, 9> scores = {2, 3, 5, 7, 11, 13, 17, 19, 23};
    int data_count = 1;
};
