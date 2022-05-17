#pragma once
#include <string>

struct Data {
public:
    Data(const std::string& url, const std::string& content)
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
