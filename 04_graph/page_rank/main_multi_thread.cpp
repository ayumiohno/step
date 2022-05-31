#include "page_rank.hpp"
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>

static std::mutex m;

void readFile(const std::string& file_name,
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
    std::unordered_map<uint32_t, std::pair<uint32_t, std::unordered_map<uint32_t, uint16_t>>> links;

    auto readFile0 = [&pages]() { readFile("../testcase/pages.txt",
                                      [&pages](auto arg1, auto arg2) { pages[(uint32_t)(std::stoi(arg1))] = arg2; }); };

    auto process_links_data = [&links](auto arg1, auto arg2) {
        uint32_t v_arg1 = (uint32_t)(std::stoi(arg1));
        uint32_t v_arg2 = (uint32_t)(std::stoi(arg2));
        m.lock();
        links[v_arg1].first = 50;
        links[v_arg1].second[v_arg2] = 0; 
        m.unlock(); };

    auto readFile1 = [&]() { readFile("../testcase/links_div00.txt", process_links_data); };
    auto readFile2 = [&]() { readFile("../testcase/links_div01.txt", process_links_data); };
    auto readFile3 = [&]() { readFile("../testcase/links_div02.txt", process_links_data); };

    std::thread th0(readFile0);
    std::thread th1(readFile1);
    std::thread th2(readFile2);
    std::thread th3(readFile3);

    th0.join();
    std::cerr << "th0 end" << std::endl;
    th1.join();
    std::cerr << "th1 end" << std::endl;
    th2.join();
    std::cerr << "th2 end" << std::endl;
    th3.join();
    std::cerr << "th3 end" << std::endl;

    updatePageRank(links, 5);

    //printAllRank(pages, links);

    printTopXPages(pages, links);

    return 0;
}
