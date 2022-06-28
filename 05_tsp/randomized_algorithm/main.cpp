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

    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_int_distribution<int> distr(1, num_of_dots - 1);

    std::mt19937 engine(rd());

    double best_score = 1000000;
    std::vector<Point> best_points;

    for (int coun = 0; coun < 5; ++coun) {

        std::cerr << coun << std::endl;
        std::shuffle(points.begin(), points.end(), engine);
        std::set<int> cands;
        std::set<int> p_cands;

        /*入力順に点を繋いでいった時の合計の長さ*/
        double min_length = 0;
        for (int i = 0; i < num_of_dots; ++i) {
            min_length += (points.at(i) - points.at((i - 1 + num_of_dots) % num_of_dots)).getNolm();
            cands.insert(i);
        }

        bool is_end = false;
        int count = 0;
        while (!is_end && count < 100000) {
            p_cands = cands;
            for (auto a : p_cands) {
                cands.erase(a);
                is_end = true;
                for (int b = 1; b < num_of_dots; ++b) {
                    ++count;
                    if (a == b) {
                        continue;
                    }
                    if (!swapPoints(std::min(a, b), std::max(a, b), points, min_length, num_of_dots, cands)) {
                        is_end = false;
                    }
                }
            }
        }
        for (int j = 0; j < 10; ++j) {
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for (int base = 1; base < num_of_dots; ++base) {
                    ++count;
                    if (!movePoint(base, points, min_length, num_of_dots, cands)) {
                        is_end = false;
                    }
                }
            }

            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for (int base = 1; base < num_of_dots; ++base) {
                    ++count;
                    if (!move2Point(base, points, min_length, num_of_dots, cands)) {
                        is_end = false;
                    }
                }
            }
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                p_cands = cands;
                for (auto a : p_cands) {
                    cands.erase(a);
                    for (int b = 1; b < num_of_dots; ++b) {
                        ++count;
                        if (a == b) {
                            continue;
                        }
                        if (!swapPoints(std::min(a, b), std::max(a, b), points, min_length, num_of_dots, cands)) {
                            is_end = false;
                        }
                    }
                }
            }
        }
        std::cerr << min_length << std::endl;
        if (min_length < best_score) {
            best_score = min_length;
            best_points = points;
        }
    }
    std::cerr << "best: " << best_score << std::endl;
    std::cout << best_points.size() << std::endl;
    for (auto point : best_points) {
        std::cout << point.x << " " << point.y << std::endl;
    }
}
