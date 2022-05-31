#include "page_rank.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>


void readFile(const std::string& file_name,
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
    std::unordered_map<uint32_t, std::string> pages;
    std::unordered_map<uint32_t, std::pair<uint32_t, std::unordered_map<uint32_t, uint16_t>>> links;

    std::cerr << "test small case? YES : 1 NO : 0" << std::endl;

    bool is_small;
    std::cin >> is_small;

    std::string filename_pages = is_small ? "../testcase/pages_small.txt" : "../testcase/pages.txt";
    std::string filename_links = is_small ? "../testcase/links_small.txt" : "../testcase/links.txt";

    readFile(filename_pages,
        [&pages](auto arg1, auto arg2) { pages[(uint32_t)(std::stoi(arg1))] = arg2; });
    readFile(filename_links,
        [&links](auto arg1, auto arg2) { 
        links[(uint32_t)(std::stoi(arg1))].first = 50;
        links[(uint32_t)(std::stoi(arg1))].second[(uint32_t)(std::stoi(arg2))] = 0; });

    std::cerr << "end reading file" << std::endl;


    updatePageRank(links, 5);

    //printAllRank(pages, links);

    printTopXPages(pages, links);

    return 0;
}
