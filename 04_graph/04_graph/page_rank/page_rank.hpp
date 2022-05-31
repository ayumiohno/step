#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

void updatePageRank(
    std::unordered_map<uint32_t,
        std::pair<uint32_t,
            std::unordered_map<uint32_t, uint16_t>>>& links,
    const std::size_t&& loop_times = 10,
    const uint8_t&& random_rate = 5);

void printAllRank(
    const std::unordered_map<uint32_t, std::string>& pages,
    std::unordered_map<uint32_t,
        std::pair<uint32_t,
            std::unordered_map<uint32_t, uint16_t>>>& links);

void printTopXPages(
    const std::unordered_map<uint32_t, std::string>& pages,
    std::unordered_map<uint32_t,
        std::pair<uint32_t,
            std::unordered_map<uint32_t, uint16_t>>>& links,
    uint8_t num_of_showing_pages = 10);
