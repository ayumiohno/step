#include "page_rank.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>

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
    std::unordered_map<uint32_t, std::string> pages;
    std::unordered_map<uint32_t, std::pair<uint16_t, std::map<uint32_t, uint8_t>>> links;

    auto readFile1 = [&pages]() { readFile("../testcase/pages_small.txt",
                                      [&pages](auto arg1, auto arg2) { pages[(uint32_t)(std::stoi(arg1))] = arg2; }); };
    auto readFile2 = [&links]() { readFile("../testcase/links_small.txt",
                                      [&links](auto arg1, auto arg2) {
        links[(uint32_t)(std::stoi(arg1))].first = 50;
        links[(uint32_t)(std::stoi(arg1))].second[(uint32_t)(std::stoi(arg2))] = 0; }); };

    std::thread th1(readFile1);
    std::thread th2(readFile2);

    th1.join();
    std::cerr << "th1 end" << std::endl;
    th2.join();
    std::cerr << "th2 end" << std::endl;

    updatePageRank(links, 5);

    //printAllRank(pages, links);

    printTopXPages(pages, links);

    return 0;
}
