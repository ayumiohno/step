#include "ga.hpp"
#include "point.hpp"
#include "tsp.hpp"
#include <array>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

constexpr int FILE_NUM = 7;
constexpr std::array<int, 8> NUMS = {5, 8, 16, 64, 128, 512, 2048, 8192};
constexpr int NUM_OF_CITY = NUMS.at(FILE_NUM);

int main()
{
    std::vector<Point> points;
    {
        auto file_name = "../testcase/input_" + std::to_string(FILE_NUM) + ".csv";
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
    GeneticAlgorithm<NUM_OF_CITY> GA{points, 100, 1000, 15, 15};
    std::cout << "best: " << GA.evolution() << std::endl;
}
