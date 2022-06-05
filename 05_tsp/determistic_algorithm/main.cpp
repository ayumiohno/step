#include "point.hpp"
#include "tsp.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    std::vector<Point> points;
    {
        auto file_name = "../google-step-tsp/input_2.csv";
        std::ifstream file(file_name);
        std::string data;
        while (std::getline(file, data)) {
            auto index = data.find(',');
            auto x = data.substr(0, index);
            auto y = data.substr(index + 1, data.size() - x.size() - 1);
            try {
                std::cerr << x << " " << std::stold(x) << " " << y << " " << std::stold(y) << std::endl;
                points.push_back(Point{(double)(std::stold(x)), (double)(std::stold(y))});
            } catch (const std::invalid_argument& e) {
                std::cout << e.what() << std::endl;
            }
        }
        file.close();
    }


    std::vector<int> usable_ids;
    for (int i = 1; i < points.size(); ++i) {
        usable_ids.push_back(i);
    }

    Table table_init{points.size()};
    for (int end_id = 1; end_id < points.size(); ++end_id) {
        table_init.pushMinDistance(end_id, {}, (points.at(0) - points.at(end_id)).getNolm());
    }
    for (int using_num_of_dots = 1; using_num_of_dots < points.size() - 1; ++using_num_of_dots) {
        increaseNumOfDot(table_init, usable_ids, using_num_of_dots, points);
    }

    finalTable(table_init, usable_ids, points);
    std::cout << table_init.getMinDistance(0, usable_ids) << std::endl;
}
