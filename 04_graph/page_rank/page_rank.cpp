#include "page_rank.hpp"
#include <assert.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>
#include <thread>


void distributePointsToEdges(
    const std::vector<std::unordered_map<std::string,
        std::pair<uint16_t,
            std::map<std::string, uint8_t>>>::iterator>& list,
    const uint8_t& random_rate,
    const uint8_t&& th_id)
{
    std::size_t node_num = list.size();
    std::size_t start = th_id * node_num / 4;
    std::size_t end = (th_id + 1) * node_num / 4;
    for (std::size_t i = start; i < end; ++i) {
        uint8_t num_of_edges = list.at(i)->second.second.size();
        if (num_of_edges != 0) {
            uint16_t& point_of_node = list.at(i)->second.first;
            uint8_t point_of_edges = (uint8_t)(point_of_node * (random_rate - 1) / random_rate / num_of_edges);
            for (auto& edge : list.at(i)->second.second) {
                edge.second = point_of_edges;
            }
        }
        list.at(i)->second.first = 0;
    }
}

void getPointsFromEdges(
    std::unordered_map<std::string,
        std::pair<uint16_t,
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
        std::pair<uint16_t,
            std::map<std::string, uint8_t>>>& links,
    const std::size_t&& loop_times,
    const uint8_t&& random_rate)
{
    std::vector<std::unordered_map<std::string,
        std::pair<uint16_t,
            std::map<std::string, uint8_t>>>::iterator>
        list;

    for (auto itr = links.begin(); itr != links.end(); ++itr) {
        list.push_back(itr);
    }

    for (std::size_t i = 0; i < loop_times; ++i) {
        std::cerr << "loop: " << i << std::endl;
        std::thread th0([&]() { distributePointsToEdges(list, random_rate, 0); });
        std::thread th1([&]() { distributePointsToEdges(list, random_rate, 1); });
        std::thread th2([&]() { distributePointsToEdges(list, random_rate, 2); });
        std::thread th3([&]() { distributePointsToEdges(list, random_rate, 3); });
        th0.join();
        std::cerr << "th0 end." << std::endl;
        th1.join();
        std::cerr << "th1 end." << std::endl;
        th2.join();
        std::cerr << "th2 end." << std::endl;
        th3.join();
        std::cerr << "th3 end." << std::endl;
        /*distributePointsToEdges(list, random_rate, 0);
        distributePointsToEdges(list, random_rate, 1);
        distributePointsToEdges(list, random_rate, 2);
        distributePointsToEdges(list, random_rate, 3);*/
        getPointsFromEdges(links, random_rate);
    }
}

void printAllRank(
    const std::unordered_map<std::string, std::string>& pages,
    std::unordered_map<std::string,
        std::pair<uint16_t,
            std::map<std::string, uint8_t>>>& links)
{
    for (const auto& link : links) {
        std::cout << pages.at(link.first) << " : " << (int)link.second.first << std::endl;
    }
}

void printTopXPages(
    const std::unordered_map<std::string, std::string>& pages,
    std::unordered_map<std::string,
        std::pair<uint16_t,
            std::map<std::string, uint8_t>>>& links,
    uint8_t num_of_showing_pages)
{
    auto greater = [](const std::pair<uint16_t, std::string>& a, const std::pair<uint16_t, std::string>& b) { return a.first > b.first; };
    std::priority_queue<std::pair<uint16_t, std::string>, std::vector<std::pair<uint16_t, std::string>>, decltype(greater)> top_x_list;

    uint8_t count = 0;
    for (const auto& link : links) {
        if (count < num_of_showing_pages) {
            top_x_list.push({link.second.first, link.first});
            ++count;
        } else {
            uint16_t point = link.second.first;
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
