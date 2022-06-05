#pragma once
#include <assert.h>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

class Table
{
public:
    Table(int num_of_points)
    {
        min_distances.resize(num_of_points);
    }
    ~Table() {}

    void pushMinDistance(int end_id, std::vector<int> passing_points, double min_distance)
    {
        /*for (auto p : passing_points) {
            std::cerr << p << " ";
        }
        std::cerr << "was in. length: " << min_distance << std::endl;*/
        this->min_distances.at(end_id)[passing_points] = min_distance;
    }

    double getMinDistance(int end_id, std::vector<int> passing_points)
    {
        /*for (auto p : passing_points) {
            std::cerr << p << " ";
        }
        std::cerr << "was out. " << std::endl;*/
        assert(this->min_distances.at(end_id).count(passing_points));
        return this->min_distances.at(end_id).at(passing_points);
    }

private:
    /* index : 最後の点のid
     * map : key -> 通過点のidの組み合わせ, value -> 距離
     * keyのvectorはsort済みにする*/
    std::vector<std::map<std::vector<int>, double>> min_distances;
};

void increaseNumOfDot(Table& table_before, const std::vector<int>& usable_ids, int& using_num_of_dots, const std::vector<Point>& points);

void finalTable(Table& table_before, const std::vector<int>& usable_ids, const std::vector<Point>& points);
