#pragma once
#include <algorithm>
#include <assert.h>
#include <string>
#include <utility>
#include <vector>

class DataBase
{
public:
    DataBase()
    {
        database.push_back({"https://a.com", "a"});
        database.push_back({"https://b.ac.jp", "b"});
        database.push_back({"https://www.go.jp", "www"});
        database.push_back({"https://abcdefg.com", "abcdefg"});
        database.push_back({"https://utxyz.go.jp", "utxyz"});
        database.push_back({"https://xyzut.go.jp", "xyzut"});
        database.push_back({"http://itc-lms.ecc.u-tokyo.ac.jp", "itc-lms"});
        std::sort(database.begin(), database.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
    }

    std::string loadContent(const std::string& url) const
    {
        auto lower_ptr = std::lower_bound(
            database.begin(),
            database.end(),
            std::pair<std::string, std::string>{url, "hoge"},
            [](const auto& a, const auto& b) { return a.first < b.first; });
        assert(lower_ptr != database.end());
        return lower_ptr->second;
    }

    std::vector<std::pair<std::string, std::string>> database;
};
