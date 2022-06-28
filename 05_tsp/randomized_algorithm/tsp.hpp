#pragma once
#include <set>
#include <string>
#include <vector>

bool swapPoints(const int& swap1, const int& swap2,
    std::vector<Point>& points, double& min_length, const int& num_of_dots, std::set<int>& cands);
bool movePoint(const int& base, std::vector<Point>& points, double& min_length, const int& num_of_dots, std::set<int>& cands);
bool move2Point(const int& base, std::vector<Point>& points, double& min_length, const int& num_of_dots, std::set<int>& cands);
