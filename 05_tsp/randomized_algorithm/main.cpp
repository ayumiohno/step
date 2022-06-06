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
        std::cerr << "put file number" << std::endl;
        std::string file_num;
        std::cin >> file_num;
        auto file_name = "../google-step-tsp/input_" + file_num + ".csv";
        std::ifstream file(file_name);
        std::string data;
        while (std::getline(file, data)) {
            auto index = data.find(',');
            auto x = data.substr(0, index);
            auto y = data.substr(index + 1, data.size() - x.size() - 1);
            try {
                points.push_back(Point{(double)(std::stold(x)), (double)(std::stold(y))});
            } catch (const std::invalid_argument& e) {
            }
        }
        file.close();
    }

    auto num_of_dots = points.size();

    /*入力順に点を繋いでいった時の合計の長さ*/
    double min_length = 0;
    for (int i = 0; i < num_of_dots; ++i) {
        min_length += (points.at(i) - points.at((i - 1 + num_of_dots) % num_of_dots)).getNolm();
    }

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr(1, num_of_dots - 1);

    for (int i = 0; i < 100000000; ++i) {
        /*入れ替える点のidを乱数で決定*/
        auto a = distr(eng);
        auto b = distr(eng);
        if (a == b) {
            continue;
        }
        swapPoints(std::min(a, b), std::max(a, b), points, min_length, num_of_dots);
    }

    std::cout << min_length << std::endl;
}
