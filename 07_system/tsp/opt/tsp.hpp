#pragma once
#include "point.hpp"
#include <set>
#include <string>
#include <vector>

double getDistance(const std::vector<Point>& points, int i, int j, int num_of_dots);

bool twoOpt(const int& swap1, const int& swap2,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots);
bool movePoint(const int& base,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots);
bool move2Points(const int& base,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots);
bool move3Points(const int& base,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots);
bool move4Points(const int& base,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots);
bool threeOpt(const int& swap1, const int& swap2, const int& swap3,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots);
