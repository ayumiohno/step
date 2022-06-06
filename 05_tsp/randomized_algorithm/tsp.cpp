#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <iostream>
#include <utility>

void swapPoints(const int& swap1, const int& swap2, 
        std::vector<Point>& points, double& min_length, const int& num_of_dots)
{
    double length_diff = 0; //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計
    length_diff -= (points.at(swap1) - points.at((swap1 - 1 + num_of_dots) % num_of_dots)).getNolm();
    length_diff += (points.at(swap2) - points.at((swap1 - 1 + num_of_dots) % num_of_dots)).getNolm();
    length_diff -= (points.at(swap2) - points.at((swap2 + 1) % num_of_dots)).getNolm();
    length_diff += (points.at(swap1) - points.at((swap2 + 1) % num_of_dots)).getNolm();
    if (length_diff < 0) { //入れ替えると長さが短くなる場合
        min_length += length_diff;
        std::reverse(points.begin() + swap1, points.begin() + swap2 + 1);
    }
}
