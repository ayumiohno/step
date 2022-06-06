#pragma once
#include <assert.h>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

class DpMemo
{
public:
    DpMemo(int num_of_points)
    {
        min_distances.resize(num_of_points);
    }
    ~DpMemo() {}

    void pushMinDistance(int end_id, std::vector<int> passing_points, double min_distance)
    {
        this->min_distances.at(end_id)[passing_points] = min_distance;
    }

    double getMinDistance(int end_id, std::vector<int> passing_points)
    {
        return this->min_distances.at(end_id).at(passing_points);
    }

private:
    /* index : 最終点のid
     * map : key -> 通過点のidの組み合わせ, value -> 距離
     * keyのvectorはsort済みとする*/
    std::vector<std::map<std::vector<int>, double>> min_distances;
};

void increaseNumOfDot(DpMemo& table_before, int& using_num_of_dots, const std::vector<Point>& points);

void finalDpMemo(DpMemo& table_before, const std::vector<Point>& points);
