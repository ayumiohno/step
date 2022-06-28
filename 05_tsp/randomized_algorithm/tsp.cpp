#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <utility>

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_int_distribution<int> distr(1, 1000);

bool isChangeRandom()
{
    return false;  //distr(eng) == 1;
}

/* 1 <= swap1, swap2  <= points.size() - 1 */
bool swapPoints(const int& swap1, const int& swap2,
    std::vector<Point>& points, double& min_length, const int& num_of_dots, std::set<int>& cands)
{
    if (swap1 == 0 || swap2 == 0 || swap1 >= num_of_dots || swap2 >= num_of_dots) {
        return true;
    }
    double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計
    length_diff -= (points.at(swap1) - points.at(swap1 - 1)).getNolm();
    length_diff += (points.at(swap2) - points.at(swap1 - 1)).getNolm();
    length_diff -= (points.at(swap2) - points.at((swap2 + 1) % num_of_dots)).getNolm();
    length_diff += (points.at(swap1) - points.at((swap2 + 1) % num_of_dots)).getNolm();
    if (length_diff < 0 || isChangeRandom()) {  //入れ替えると長さが短くなる場合
        min_length += length_diff;
        std::reverse(points.begin() + swap1, points.begin() + swap2 + 1);
        cands.insert(swap1);
        cands.insert(swap1 + 1);
        cands.insert(swap2);
        cands.insert(swap2 + 1);
        return false;
    }
    return true;
}


bool movePoint(const int& base, std::vector<Point>& points, double& min_length, const int& num_of_dots, std::set<int>& cands)
{
    double min_length_diff = 0;
    int min_i;
    for (int i = 1; i <= num_of_dots; ++i) {
        double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計
        if (std::abs(base - i) <= 2) {
            continue;
        }
        length_diff += (points.at(base) - points.at(i - 1)).getNolm();
        length_diff += (points.at(base) - points.at(i % num_of_dots)).getNolm();
        length_diff -= (points.at(base) - points.at(base - 1)).getNolm();
        length_diff -= (points.at(base) - points.at((base + 1) % num_of_dots)).getNolm();
        length_diff += (points.at(base - 1) - points.at((base + 1) % num_of_dots)).getNolm();
        length_diff -= (points.at(i - 1) - points.at(i % num_of_dots)).getNolm();
        if (length_diff < min_length_diff) {
            min_i = i;
            min_length_diff = length_diff;
        }
    }
    if (min_length_diff < 0 || isChangeRandom()) {
        min_length += min_length_diff;
        auto base_point = points.at(base);
        points.erase(points.begin() + base);
        int new_i = base < min_i ? min_i - 1 : min_i;
        points.insert(points.begin() + new_i, base_point);
        cands.insert(base);
        cands.insert((base + 1) % num_of_dots);
        cands.insert(new_i);
        cands.insert((new_i + 1) % num_of_dots);

        return false;
    }
    return true;
}

bool move2Point(const int& base, std::vector<Point>& points, double& min_length, const int& num_of_dots, std::set<int>& cands)
{
    if (base == num_of_dots - 1) {
        return false;
    }
    double min_length_diff = 0;
    int min_i;
    bool muki;
    for (int i = 1; i <= num_of_dots; ++i) {
        double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計
        double length_diff1 = 0;
        double length_diff2 = 0;
        if (std::abs(base - i) <= 2) {
            continue;
        }
        length_diff -= (points.at(base - 1) - points.at(base)).getNolm();
        length_diff -= (points.at((base + 1) % num_of_dots) - points.at((base + 2) % num_of_dots)).getNolm();
        length_diff += (points.at(base - 1) - points.at((base + 2) % num_of_dots)).getNolm();
        length_diff -= (points.at(i - 1) - points.at(i % num_of_dots)).getNolm();
        length_diff1 += (points.at(base) - points.at(i - 1)).getNolm();
        length_diff1 += (points.at((base + 1) % num_of_dots) - points.at(i % num_of_dots)).getNolm();
        length_diff2 += (points.at(base) - points.at(i % num_of_dots)).getNolm();
        length_diff2 += (points.at((base + 1) % num_of_dots) - points.at(i - 1)).getNolm();

        if (length_diff1 < length_diff2) {
            length_diff += length_diff1;
            if (length_diff < min_length_diff) {
                min_i = i;
                min_length_diff = length_diff;
                muki = true;
            }
        } else {
            length_diff += length_diff2;
            if (length_diff < min_length_diff) {
                min_i = i;
                min_length_diff = length_diff;
                muki = false;
            }
        }
    }
    if (min_length_diff < 0 || isChangeRandom()) {
        min_length += min_length_diff;
        auto base_point1 = points.at(base);
        auto base_point2 = points.at(base + 1);
        points.erase(points.begin() + base + 1);
        points.erase(points.begin() + base);
        int new_i = base < min_i ? min_i - 2 : min_i;
        if (muki) {
            points.insert(points.begin() + new_i, base_point2);
            points.insert(points.begin() + new_i, base_point1);
        } else {
            points.insert(points.begin() + new_i, base_point1);
            points.insert(points.begin() + new_i, base_point2);
        }
        cands.insert(base);
        cands.insert((base + 1) % num_of_dots);
        cands.insert(base - 1);
        cands.insert((base + 2) % num_of_dots);
        cands.insert(new_i);
        cands.insert((new_i + 1) % num_of_dots);
        cands.insert(new_i - 1);
        cands.insert((new_i + 2) % num_of_dots);
        return false;
    }
    return true;
}
