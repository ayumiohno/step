#include "path_finding.hpp"
#include <functional>
#include <queue>
#include <stack>

/*valueをkey(id)に変換*/
auto findKey(const std::string& value,
    const std::unordered_map<uint32_t, std::string>& pages)
{
    for (const auto& page : pages) {
        if (page.second == value) {
            return page.first;
        }
    }
    NoSuchValueException error{value};
    throw error;
}

void depthFirstSearch(const std::string& start_value,
    const std::string& goal_value,
    const std::unordered_map<uint32_t, std::string>& pages,
    const std::unordered_map<uint32_t, std::unordered_set<uint32_t>>& links)
{
    /*valueをkey(id)に変換*/
    uint32_t start_key;
    uint32_t goal_key;
    try {
        start_key = findKey(start_value, pages);
        goal_key = findKey(goal_value, pages);
    } catch (NoSuchValueException& error) {
        throw error;
    }

    std::stack<uint32_t> list;                 //処理中のnode
    std::unordered_set<uint32_t> is_searched;  //訪問済みのnode

    list.push(start_key);
    is_searched.insert(start_key);

    bool is_found = false;
    while (!list.empty() && !is_found) {
        auto parent = list.top();
        if (links.count(parent)) {
            auto is_node_exist = false;
            for (auto link : links.at(parent)) {
                if (link == goal_key) {
                    is_searched.insert(link);
                    list.push(link);
                    is_found = true;
                    is_node_exist = true;
                    break;
                }
                if (!is_searched.count(link)) {
                    is_searched.insert(link);
                    list.push(link);
                    is_node_exist = true;
                    break;
                }
            }
            if (!is_node_exist) {
                list.pop();
            }
        } else {
            list.pop();
        }
    }

    /*path 表示 = 現在残っているstackの中身を表示*/
    if (is_found) {
        while (!list.empty()) {
            std::cout << pages.at(list.top()) << " ";
            list.pop();
        }
        std::cout << std::endl;
    } else {
        std::cout << "No Path Found." << std::endl;
    }
}

void breadthFirstSearch(const std::string& start_value,
    const std::string& goal_value,
    const std::unordered_map<uint32_t, std::string>& pages,
    const std::unordered_map<uint32_t, std::unordered_set<uint32_t>>& links)
{
    /*valueをkey(id)に変換*/
    uint32_t start_key;
    uint32_t goal_key;
    try {
        start_key = findKey(start_value, pages);
        goal_key = findKey(goal_value, pages);
    } catch (NoSuchValueException error) {
        throw error;
    }

    std::queue<uint32_t> list;                           //処理中のnode(同じ深さ)
    std::unordered_map<uint32_t, uint32_t> parent_list;  //訪問済みnodeとその前に通ったnodeのpair

    list.push(start_key);
    parent_list[start_key] = 1 << 30;  //root : 十分大きいid

    /*BFS*/
    bool is_goal_found = false;
    while (!list.empty() && !is_goal_found) {
        auto parent = list.front();
        list.pop();
        if (links.count(parent)) {
            for (auto link : links.at(parent)) {
                if (link == goal_key) {
                    parent_list[link] = parent;
                    list.push(link);
                    is_goal_found = true;
                    break;
                }
                if (!parent_list.count(link)) {
                    parent_list[link] = parent;
                    list.push(link);
                }
            }
        }
    }

    /*parentをたどってpath 表示*/
    if (is_goal_found) {
        auto node = goal_key;
        while (node != 1 << 30) {
            std::cout << pages.at(node) << " ";
            node = parent_list.at(node);
        }
        std::cout << std::endl;
    } else {
        std::cout << "No Path Found." << std::endl;
    }
}
