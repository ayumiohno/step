#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

int main()
{
    std::vector<Point> best_points;
    std::vector<Point> init_best_points;
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
                best_points.push_back(Point{(double)(std::stold(x)), (double)(std::stold(y))});
            } catch (const std::invalid_argument& e) {
            }
        }
        file.close();
    }

    int num_of_dots = best_points.size();

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr(1, num_of_dots - 1);

    std::mt19937 engine(rd());

    double best_score = 1000000;
    std::vector<Point> best_best_points;

    for (int cycle = 0; cycle < 100; ++cycle) {

        std::cerr << cycle << std::endl;
        std::shuffle(best_points.begin(), best_points.end(), engine);
        init_best_points = best_points;

        /*入力順に点を繋いでいった時の合計の長さ*/
        double best_distance = 0;
        for (int i = 0; i < num_of_dots; ++i) {
            best_distance += getDistance(best_points, i, i + 1, num_of_dots);
        }

        bool is_end = false;
        int count = 0;
        while (!is_end && count < 10000000) {
            for(int a = 1; a < num_of_dots; ++a){
                is_end = true;
                for (int b = a + 1; b < num_of_dots; ++b) {
                    ++count;
                    if (reverseSubPath(a, b, best_points, best_distance, num_of_dots)) {
                        is_end = false;
                    }
                }
            }
        }
        std::cerr << is_end << std::endl;
        for (int j = 0; j < 5; ++j) {
        std::cerr << j  << std::endl;
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for(int base = 0; base < num_of_dots; ++base){
                    ++count;
                    if (movePoint(base, best_points, best_distance, num_of_dots)) {
                        is_end = false;
                    }
                }
            }

            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for(int base = 1; base < num_of_dots; ++base){
                    ++count;
                    if (move2best_points(base, best_points, best_distance, num_of_dots)) {
                        is_end = false;
                    }
                }
            }
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for(int base = 1; base < num_of_dots; ++base){
                    ++count;
                    if (move3best_points(base, best_points, best_distance, num_of_dots)) {
                        is_end = false;
                    }
                }
            }
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for(int base = 1; base < num_of_dots; ++base){
                    ++count;
                    if (move4best_points(base, best_points, best_distance, num_of_dots)) {
                        is_end = false;
                    }
                }
            }
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for(int a = 1; a < num_of_dots; ++a){
                    for (int b = a + 1; b < num_of_dots; ++b) {
                        ++count;
                        if(reverseSubPath(a, b, best_points, best_distance, num_of_dots)) {
                            is_end = false;
                        }
                    }
                }
            }

            count = 0;
            is_end = false;
            while (!is_end && count < 1) {
                is_end = true;
                for(int a = 0; a < num_of_dots; ++a){
                    for (int b = a + 1; b < num_of_dots; ++b) {
                        for (int c = b + 1; c < num_of_dots; ++c) {
                            ++count;
                            if(reverseSubPath3(a, b, c, best_points, best_distance, num_of_dots)) {
                                is_end = false;
                            }
                        }
                    }
                }  
            }
        }
        std::cerr << best_distance << std::endl;
        if (best_distance < best_score) {
            best_score = best_distance;
            best_best_points = best_points;
            std::cout << best_distance << std::endl;           
            for (auto point : best_best_points) {
                std::cout << point.x << " " << point.y << std::endl;
            }
            std::cout << "init: " << std::endl;
            for (auto point : init_best_points) {
                std::cout << point.x << " " << point.y << std::endl;
            }
        }
    }

    std::cerr << "best: " << best_score << std::endl;
    std::cout << best_best_points.size() << std::endl;
    for (auto point : best_best_points) {
        std::cout << point.x << " " << point.y << std::endl;
    }
}