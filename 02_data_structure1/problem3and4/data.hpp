#pragma once
#include <string>

struct Data {
public:
    Data(std::string url, std::string content)
        : url(url), content(content) {}

    ~Data() {}

    auto showContent()
    {
        return this->content;
    }

    auto getUrl()
    {
        return this->url;
    }

private:
    std::string url;
    std::string content;
};
