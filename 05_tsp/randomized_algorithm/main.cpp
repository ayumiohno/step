#include "point.hpp"
#include "tsp.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    std::vector<Point> points;
    {
        auto file_name = "../google-step-tsp/input_6.csv";
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

    auto num_of_dots = points.size();

    double min_length = 0;
    for (int i = 0; i < num_of_dots; ++i) {
        min_length += (points.at(i) - points.at((i - 1 + num_of_dots) % num_of_dots)).getNolm();
    }

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr(0, num_of_dots - 1);

    for (int i = 0; i < 1000000; ++i) {
        auto a = distr(eng);
        auto b = distr(eng);
        if (a == b || std::min(a, b) == 0) {
            continue;
        }
        swap(std::min(a, b), std::max(a, b), points, min_length, num_of_dots);
    }
    std::cout << min_length << std::endl;
}
