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
    std::unordered_map<uint32_t, std::pair<uint16_t, std::unordered_map<uint32_t, uint8_t>>> links;

    auto readFile5 = [&pages]() { readFile("../testcase/pages_small.txt",
                                      [&pages](auto arg1, auto arg2) { pages[(uint32_t)(std::stoi(arg1))] = arg2; }); };

    auto process_links_data = [&links](auto arg1, auto arg2) {
        m.lock();
        links[(uint32_t)(std::stoi(arg1))].first = 50;
        links[(uint32_t)(std::stoi(arg1))].second[(uint32_t)(std::stoi(arg2))] = 0; 
        m.unlock(); };

    auto readFile0 = [&]() { readFile("../testcase/links_small_div00.txt", process_links_data); };
    auto readFile1 = [&]() { readFile("../testcase/links_small_div01.txt", process_links_data); };
    auto readFile2 = [&]() { readFile("../testcase/links_small_div02.txt", process_links_data); };
    auto readFile3 = [&]() { readFile("../testcase/links_small_div03.txt", process_links_data); };
    auto readFile4 = [&]() { readFile("../testcase/links_small_div04.txt", process_links_data); };

    std::thread th(readFile5);
    std::thread th0(readFile0);
    std::thread th1(readFile1);
    std::thread th2(readFile2);
    std::thread th3(readFile3);
    std::thread th4(readFile4);

    th.join();
    std::cerr << "th end" << std::endl;
    th0.join();
    th1.join();
    th2.join();
    th3.join();
    th4.join();

    updatePageRank(links, 5);

    //printAllRank(pages, links);

    printTopXPages(pages, links);

    return 0;
}
