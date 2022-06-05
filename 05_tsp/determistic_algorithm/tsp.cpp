#include "point.hpp"
#include "tsp.hpp"

void updateTable(const std::vector<int>& passing_points, const int& end_id, const std::vector<Point>& points, Table& table_before, Table& table_new)
{
    double min_length = 1 << 30;
    /*std::cerr << "passing_points: ";
    for (auto p : passing_points) {
        //std::cerr << p << " ";
    }
    std::cerr << "end_id: " << end_id << std::endl;*/
    for (int i = 0; i < passing_points.size(); ++i) {
        auto v = passing_points;
        v.erase(v.begin() + i);
        double length = table_before.getMinDistance(passing_points.at(i), v)
                        + (points.at(passing_points.at(i)) - points.at(end_id)).getNolm();
        min_length = std::min(min_length, length);
    }
    table_new.pushMinDistance(end_id, passing_points, min_length);  //length passing_points
}

void getDotIdsLoop(int starting_id,
    const std::vector<int>& usable_ids,
    std::vector<int> passing_points,
    const int& using_num_of_dots,
    const int& end_id, const std::vector<Point>& points,
    Table& table_before, Table& table_new)
{
    passing_points.push_back(starting_id);
    if (passing_points.size() == using_num_of_dots) {
        updateTable(passing_points, end_id, points, table_before, table_new);
        return;
    }
    //auto i_limit = usable_ids.size() - (using_num_of_dots - passing_points.size());
    for (int i = starting_id + 1; i < points.size(); ++i) {
        if (i == end_id) {
            continue;
        }
        getDotIdsLoop(i, usable_ids, passing_points, using_num_of_dots, end_id, points, table_before, table_new);
    }
}

void increaseNumOfDot(Table& table_before, const std::vector<int>& usable_ids, int& using_num_of_dots, const std::vector<Point>& points)
{
    std::cerr << "increase Num: " << using_num_of_dots << std::endl;
    Table table_new{points.size()};
    for (int end_id = 1; end_id < points.size(); ++end_id) {
        for (int starting_id = 1; starting_id < points.size(); ++starting_id) {
            if (starting_id == end_id) {
                continue;
            }
            getDotIdsLoop(starting_id, usable_ids, {}, using_num_of_dots, end_id, points, table_before, table_new);
        }
    }
    table_before = table_new;
}

void finalTable(Table& table_before, const std::vector<int>& usable_ids, const std::vector<Point>& points)
{
    std::cerr << "increase Num: "
              << "final" << std::endl;
    Table table_new{1};
    for (int starting_id = 1; starting_id < points.size(); ++starting_id) {
        getDotIdsLoop(starting_id, usable_ids, {}, points.size() - 1, 0, points, table_before, table_new);
    }
    table_before = table_new;
}
