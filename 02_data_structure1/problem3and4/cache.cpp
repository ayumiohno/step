#pragma once
#include "data.cpp"
#include "list.cpp"

//hase table の子クラスdehanaku 直接作る
template <class Num>
struct Cache {

    Cache()
    {
    }

    //どのarrayを参照するか探す(Listを返す)!
    int(const Data& data)
    {
        auto url = data.url;
        auto score = 1;
        for (auto c : url) {
            score = score * (scores.at(int(c - 'a') % 9) % Num + 1);
        }
        return score - 1;
    }
    std::array<Hash, Num> hash_table;
    constexpr std::array<int, 9> scores = {2, 3, 5, 7, 11, 13, 17, 19, 23};
};
