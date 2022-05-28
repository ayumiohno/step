#include "path_finding.hpp"
#include <assert.h>
#include <functional>
#include <queue>
#include <stack>

auto findKey(const std::string& value,
    const std::unordered_map<std::string, std::string>& pages)
{
    for (const auto& page : pages) {
        if (page.second == value) {
            return page.first;
        }
    }
    NoSuchValueException error{value};
    throw error;
}

template <class LIST>
void showPath(const std::unordered_map<std::string, std::string>& pages,
    LIST& list,
    const std::function<std::string(LIST&)>& get_first)
{
    while (!list.empty()) {
        std::cout << pages.at(get_first(list)) << std::endl;
        list.pop();
    }
}

void depthFirstSearch(const std::string& start_value,
    const std::string& goal_value,
    const std::unordered_map<std::string, std::string>& pages,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& links)
{
    std::string start_key;
    std::string goal_key;
    try {
        start_key = findKey(start_value, pages);
        goal_key = findKey(goal_value, pages);
    } catch (NoSuchValueException error) {
        throw error;
    }

    auto get_first = [](std::stack<std::string>& list) { return list.top(); };
    std::stack<std::string> list;

    std::unordered_set<std::string> is_searched;
    list.push(start_key);
    is_searched.insert(start_key);

    while (!list.empty()) {
        auto v = list.top();
        if (links.count(v)) {
            auto is_found = false;
            for (auto link : links.at(v)) {
                if (link == goal_key) {
                    is_searched.insert(link);
                    list.push(link);
                    is_found = true;
                    std::cout << "START!" << std::endl;
                    showPath<std::stack<std::string>>(pages, list, get_first);
                    std::cout << "GOAL!" << std::endl;
                    break;
                }
                if (!is_searched.count(link)) {
                    is_searched.insert(link);
                    list.push(link);
                    is_found = true;
                    break;
                }
            }
            if (!is_found) {
                list.pop();
            }
        } else {
            list.pop();
        }
    }
}

void breadthFirstSearch(const std::string& start_value,
    const std::string& goal_value,
    const std::unordered_map<std::string, std::string>& pages,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& links)
{
    std::string start_key;
    std::string goal_key;
    try {
        start_key = findKey(start_value, pages);
        goal_key = findKey(goal_value, pages);
    } catch (NoSuchValueException error) {
        throw error;
    }

    auto get_first = [](std::queue<std::string>& list) { return list.front(); };
    std::queue<std::string> list;

    std::unordered_map<std::string, std::string> parent;
    list.push(start_key);
    parent[start_key] = "\n";

    bool is_found = false;
    while (!list.empty() && !is_found) {
        auto v = list.front();
        if (links.count(v)) {
            for (auto link : links.at(v)) {
                if (link == goal_key) {
                    parent[link] = v;
                    list.push(link);
                    is_found = true;
                    break;
                }
                if (!parent.count(link)) {
                    parent[link] = v;
                    list.push(link);
                }
            }
            list.pop();
        }
    }
    if (is_found) {
        auto node = goal_key;
        while (node != "\n") {
            std::cout << pages.at(node) << std::endl;
            node = parent.at(node);
        }
    }
}
