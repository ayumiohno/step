#include "path_finding.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

void readFile(const std::string& file_name,
    const std::function<void(std::string, std::string)>&& process_data)
{
    std::ifstream file(file_name);
    std::string data;
    while (std::getline(file, data)) {
        auto index = data.find('\t');
        auto arg1 = data.substr(0, index);
        auto arg2 = data.substr(index + 1, data.size() - arg1.size() - 1);
        process_data(std::move(arg1), std::move(arg2));  //読み取ったstringの処理を記述
    }
    file.close();
}

int main()
{
    std::unordered_map<uint32_t, std::string> pages;
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> links;

    std::cerr << "test small case? YES : 1 NO : 0" << std::endl;

    bool is_small;
    std::cin >> is_small;

    std::string filename_pages = is_small ? "../testcase/pages_small.txt" : "../testcase/pages.txt";
    std::string filename_links = is_small ? "../testcase/links_small.txt" : "../testcase/links.txt";

    readFile(filename_pages,
        [&pages](auto arg1, auto arg2) { pages[(uint32_t)(std::stoi(arg1))] = arg2; });
    readFile(filename_links,
        [&links](auto arg1, auto arg2) { links[(uint32_t)(std::stoi(arg1))].insert((uint32_t)(std::stoi(arg2))); });

    std::cerr << "end reading file" << std::endl;

    while (true) {
        std::string start_value, goal_value;
        std::cerr << "start: " << std::endl;
        std::cin >> start_value;
        std::cerr << "goal: " << std::endl;
        std::cin >> goal_value;

        try {
            depthFirstSearch(start_value, goal_value, pages, links);
            std::cerr << "end dfs" << std::endl;
            breadthFirstSearch(start_value, goal_value, pages, links);
            std::cerr << "end bfs" << std::endl;
        } catch (NoSuchValueException& error) {
            error.printError();
        }

        std::cerr << "exit? YES : 1 No : 0" << std::endl;
        bool is_exit;
        std::cin >> is_exit;
        if (is_exit) {
            break;
        }
    }

    return 0;
}
