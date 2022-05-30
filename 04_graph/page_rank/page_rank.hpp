#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

void updatePageRank(
    std::unordered_map<std::string,
        std::pair<int,
            std::map<std::string, uint8_t>>>& links,
    const std::size_t&& loop_times = 10,
    const uint8_t&& random_rate = 5);

void printAllRank(
    const std::unordered_map<std::string, std::string>& pages,
    std::unordered_map<std::string,
        std::pair<int,
            std::map<std::string, uint8_t>>>& links);

void printTopXPages(
    const std::unordered_map<std::string, std::string>& pages,
    std::unordered_map<std::string,
        std::pair<int,
            std::map<std::string, uint8_t>>>& links,
    int num_of_showing_pages = 10);
