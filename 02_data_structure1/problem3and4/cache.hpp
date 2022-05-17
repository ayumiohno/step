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

    //urlからhash値を求める
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
        //order_listの先頭削除(MEMORY_SIZE以上のデータが入っているとき)
        if (data_count >= MEMORY_SIZE) {
            order_list->deleteFirst();
        } else {
            ++data_count;
        }
        //databaseからcontent読み込み
        auto content = database.loadContent(url);
        std::cout << "!!!REASULT!!! " << content << std::endl;
        //dataとnode作成
        auto& hash_list = hash_table.at(hash_value);
        auto new_node = std::make_shared<Node>(std::make_shared<Data>(url, content), hash_list, order_list);
        //order_listの最後に追加
        order_list->addLast(new_node);
        //hash値に対応するhash_listの最後に追加
        hash_list->addLast(new_node);
    }

    void processData(std::string url)
    {
        int hash_value = getHash(url);
        //hash値に対応するhash_listに既にurlがあるか探索
        auto node = hash_table.at(hash_value)->find(url);
        //なかった時
        if (node == nullptr) {
            std::cout << "create new data" << std::endl;
            processNewData(url, hash_value);
            //あった時
        } else {
            std::cout << "url already exist" << std::endl;
            //content表示
            node->showContent();
            //nodeをorder_listの最後へ
            node->goToLast(Tag::ORDER);
        }
    }

    std::array<std::shared_ptr<List>, TABLE_SIZE> hash_table;
    std::shared_ptr<List> order_list;
    DataBase database;
    const std::array<int, 9> scores = {2, 3, 5, 7, 11, 13, 17, 19, 23};
    int data_count = 0;
};
