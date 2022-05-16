#pragma once
#include <string>

struct Data {
public:
    Data(const std::string& url,
        const std::string& content)
        : url(std::move(url)), content(std::move(content)) {}

    ~Data();

    auto getContent()
    {
        return content;
    }

private:
    std::string url;
    std::string content;
};
