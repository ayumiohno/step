#include "page_rank.hpp"
#include <assert.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>

void distributePointsToEdges(
    std::unordered_map<std::string,
        std::pair<int,
            std::map<std::string, uint8_t>>>& links,
    const uint8_t& random_rate)
{
    for (auto& link : links) {
        int point_of_node = link.second.first;
        int num_of_edges = link.second.second.size();
        for (auto& edge : link.second.second) {
            assert(num_of_edges != 0);
            edge.second = point_of_node * (random_rate - 1) / random_rate / num_of_edges;
        }
        link.second.first = 0;
    }
}

void getPointsFromEdges(
    std::unordered_map<std::string,
        std::pair<int,
            std::map<std::string, uint8_t>>>& links,
    const uint8_t& random_rate)
{
    for (auto& link : links) {
        link.second.first += 50 / random_rate;  //50 : 1 nodeあたりの平均point
        for (auto& edge : link.second.second) {
            links[edge.first].first += edge.second;
        }
    }
}

void updatePageRank(
    std::unordered_map<std::string,
        std::pair<int,
            std::map<std::string, uint8_t>>>& links,
    const std::size_t&& loop_times,
    const uint8_t&& random_rate)
{
    for (std::size_t i = 0; i < loop_times; ++i) {
        std::cerr << "loop: " << i << std::endl;
        distributePointsToEdges(links, random_rate);
        getPointsFromEdges(links, random_rate);
    }
}

void printAllRank(
    const std::unordered_map<std::string, std::string>& pages,
    std::unordered_map<std::string,
        std::pair<int,
            std::map<std::string, uint8_t>>>& links)
{
    for (const auto& link : links) {
        std::cout << pages.at(link.first) << " : " << (int)link.second.first << std::endl;
    }
}

void printTopXPages(
    const std::unordered_map<std::string, std::string>& pages,
    std::unordered_map<std::string,
        std::pair<int,
            std::map<std::string, uint8_t>>>& links,
    int num_of_showing_pages)
{
    auto greater = [](const std::pair<int, std::string>& a, const std::pair<int, std::string>& b) { return a.first > b.first; };
    std::priority_queue<std::pair<int, std::string>, std::vector<std::pair<int, std::string>>, decltype(greater)> top_x_list;
    int count = 0;
    for (const auto& link : links) {
        if (count < 10) {
            top_x_list.push({link.second.first, link.first});
            ++count;
        } else {
            int point = link.second.first;
            if (point > top_x_list.top().first) {
                top_x_list.pop();
                top_x_list.push({point, link.first});
            }
        }
    }
    while (!top_x_list.empty()) {
        std::cout << pages.at(top_x_list.top().second) << " : " << (int)top_x_list.top().first << std::endl;
        top_x_list.pop();
    }
    std::cerr << "end" << std::endl;
}
