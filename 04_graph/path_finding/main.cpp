#include "path_finding.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

void readFile(const std::string&& file_name,
    const std::function<void(std::string, std::string)>&& process_data)
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
    std::unordered_map<std::string, std::string> pages;
    std::unordered_map<std::string, std::unordered_set<std::string>> links;
    readFile("../testcase/pages.txt", [&pages](auto arg1, auto arg2) { pages[arg1] = arg2; });
    readFile("../testcase/links.txt", [&links](auto arg1, auto arg2) { links[arg1].insert(arg2); });


    /*for (const auto& page : pages) {
        if (page.second == "Google") {
            std::cout << page.second << " " << page.first << std::endl;
            break;
        }
    }*/

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
            breadthFirstSearch(start_value, goal_value, pages, links);
        } catch (NoSuchValueException& error) {
            error.printError();
        }
    }

    return 0;
}
