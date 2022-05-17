#pragma once
#include <algorithm>
#include <assert.h>
#include <string>
#include <vector>

class DataBase
{
public:
    DataBase()
    {
        database.push_back({"https://a.com", "a"});
        database.push_back({"https://b.ac.jp", "b"});
        database.push_back({"https://www.go.jp", "c"});
        database.push_back({"https://abcdefg.com", "d"});
        database.push_back({"https://utxyz.go.jp", "e"});
        database.push_back({"https://xyzut.go.jp", "e"});
        std::sort(database.begin(), database.end(), [](auto& a, auto& b) { return a.first < b.first; });
    }

    std::string loadContent(std::string url)
    {
        auto lower_ptr = std::lower_bound(
            database.begin(),
            database.end(),
            std::pair<std::string, std::string>{url, "hoge"},
            [](auto& a, auto& b) { return a.first < b.first; });
        //assert(lower_ptr != nullptr);
        return lower_ptr->second;  //TODO tukaikatatigau
    }

    std::vector<std::pair<std::string, std::string>> database;
};
