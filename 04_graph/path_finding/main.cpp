#include "path_finding.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

void readFile(const std::string&& file_name,
    const std::function<void(std::string, std::string)>& process_data)
{
    std::ifstream file(file_name);
    std::string data;
    while (std::getline(file, data)) {
        auto index = data.find('\t');
        auto arg1 = data.substr(0, index);
        auto arg2 = data.substr(index + 1, data.size() - arg1.size() - 1);
        process_data(std::move(arg1), std::move(arg2));
    }
    file.close();
}

int main()
{
    std::unordered_map<uint32_t, std::string> pages;
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> links;
    readFile("../testcase/pages.txt",
        [&pages](auto arg1, auto arg2) { pages[(uint32_t)(std::stoi(arg1))] = arg2; });
    readFile("../testcase/links.txt",
        [&links](auto arg1, auto arg2) { links[(uint32_t)(std::stoi(arg1))].insert((uint32_t)(std::stoi(arg2))); });

    std::cerr << "end reading file" << std::endl;

    while (true) {
        std::string start_value, goal_value;
        std::cin >> start_value;
        if (start_value == "0") {
            std::cerr << "exit" << std::endl;
            break;
        }
        std::cin >> goal_value;
        try {
            depthFirstSearch(start_value, goal_value, pages, links);
            std::cerr << "end dfs" << std::endl;
            breadthFirstSearch(start_value, goal_value, pages, links);
            std::cerr << "end bfs" << std::endl;
        } catch (NoSuchValueException& error) {
            error.printError();
        }
    }

    return 0;
}
