#include "page_rank.hpp"
#include <assert.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>
#include <thread>


void distributePointsToEdges(
    std::unordered_map<uint32_t,
        std::pair<uint32_t,
            std::unordered_map<uint32_t, uint16_t>>>& links,
    const uint8_t& random_rate)  //何分の一をrandomに振り分けるか
{
    for (auto& link : links) {
        uint16_t num_of_edges = link.second.second.size();
        if (num_of_edges != 0) {
            uint32_t& point_of_node = link.second.first;
            uint16_t point_of_edges = (uint16_t)(point_of_node * (random_rate - 1) / random_rate / num_of_edges);
            for (auto& edge : link.second.second) {
                edge.second = point_of_edges;
            }
        }
        link.second.first = 0;
    }
}

void sumPointsFromEdges(
    std::unordered_map<uint32_t,
        std::pair<uint32_t,
            std::unordered_map<uint32_t, uint16_t>>>& links,
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
    std::unordered_map<uint32_t,
        std::pair<uint32_t,
            std::unordered_map<uint32_t, uint16_t>>>& links,
    const std::size_t&& loop_times,
    const uint8_t&& random_rate)
{
    for (std::size_t i = 0; i < loop_times; ++i) {
        std::cerr << "loop: " << i << std::endl;
        distributePointsToEdges(links, random_rate);
        std::cerr << "end distribution" << std::endl;
        sumPointsFromEdges(links, random_rate);
        std::cerr << "end summation" << std::endl;
    }
}

void printAllRank(
    const std::unordered_map<uint32_t, std::string>& pages,
    std::unordered_map<uint32_t,
        std::pair<uint32_t,
            std::unordered_map<uint32_t, uint16_t>>>& links)
{
    for (const auto& link : links) {
        std::cout << pages.at(link.first) << " : " << (int)link.second.first << std::endl;
    }
}

void printTopXPages(
    const std::unordered_map<uint32_t, std::string>& pages,
    std::unordered_map<uint32_t,
        std::pair<uint32_t,
            std::unordered_map<uint32_t, uint16_t>>>& links,
    uint8_t num_of_showing_pages)
{
    auto greater = [](const std::pair<uint32_t, uint32_t>& a,
                       const std::pair<uint32_t, uint32_t>& b) { return a.first > b.first; };
    std::priority_queue<std::pair<uint32_t, uint32_t>,
        std::vector<std::pair<uint32_t, uint32_t>>,
        decltype(greater)>
        top_x_list;  //top x個のpoint, key(id)を保存

    //top_x_listの作成
    uint8_t count = 0;
    for (const auto& link : links) {
        if (count < num_of_showing_pages) {
            top_x_list.push({link.second.first, link.first});
            ++count;
        } else {
            uint32_t point = link.second.first;
            if (point > top_x_list.top().first) {
                top_x_list.pop();
                top_x_list.push({point, link.first});
            }
        }
    }

    //top_x_listの出力
    while (!top_x_list.empty()) {
        std::cout << pages.at(top_x_list.top().second) << " : " << (int)top_x_list.top().first << std::endl;
        top_x_list.pop();
    }
    std::cerr << "end" << std::endl;
}
