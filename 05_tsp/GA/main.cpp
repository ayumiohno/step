#include "framework.hpp"
#include "point.hpp"
#include "tsp.hpp"
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

int main()
{

    std::vector<Point> points;
    std::vector<Point> init_points;
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
    GeneticAlgorithm GA{points, 100, 1000, 20, 20};
    std::cout << GA.evolution() << std::endl;
}
