#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

int main()
{
    int num_of_dots;
    std::vector<Point> points(num_of_dots);

    std::cin >> num_of_dots;

    for (int i = 0; i < num_of_dots; ++i) {
        double x, y;
        std::cin >> x >> y;
        points.at(i).x = x;
        points.at(i).y = y;
    }


    /*入力順に点を繋いでいった時の合計の長さ*/
    double min_length = 0;
    for (int i = 0; i < num_of_dots; ++i) {
        min_length += getDistance(points, i, i + 1, num_of_dots);
    }

    for (int j = 0; j < 3; ++j) {
        bool is_end = true;
        for (int a = 0; a < num_of_dots; ++a) {
            for (int b = a + 1; b < num_of_dots; ++b) {
                for (int c = b + 1; c < num_of_dots; ++c) {
                    if (reverseSubPath3(a, b, c, points, min_length, num_of_dots)) {
                        is_end = false;
                    }
                }
            }
        }

        if (is_end) {
            break;
        }

        is_end = false;
        while (!is_end) {
            is_end = true;
            for (int base = 0; base < num_of_dots; ++base) {
                if (movePoint(base, points, min_length, num_of_dots)) {
                    is_end = false;
                }
            }
        }

        is_end = false;
        while (!is_end) {
            is_end = true;
            for (int base = 1; base < num_of_dots; ++base) {
                if (move2Points(base, points, min_length, num_of_dots)) {
                    is_end = false;
                }
            }
        }
        is_end = false;
        while (!is_end) {
            is_end = true;
            for (int base = 1; base < num_of_dots; ++base) {
                if (move3Points(base, points, min_length, num_of_dots)) {
                    is_end = false;
                }
            }
        }
        is_end = false;
        while (!is_end) {
            is_end = true;
            for (int base = 1; base < num_of_dots; ++base) {
                if (move4Points(base, points, min_length, num_of_dots)) {
                    is_end = false;
                }
            }
        }
        is_end = false;
        while (!is_end) {
            is_end = true;
            for (int a = 1; a < num_of_dots; ++a) {
                for (int b = a + 1; b < num_of_dots; ++b) {
                    if (reverseSubPath(a, b, points, min_length, num_of_dots)) {
                        is_end = false;
                    }
                }
            }
        }
        std::cerr << min_length << std::endl;
        std::cout << min_length << std::endl;
        for (auto point : points) {
            std::cout << point.x << " " << point.y << std::endl;
        }
    }
    std::cerr << min_length << std::endl;
    std::cout << min_length << std::endl;
    for (auto point : points) {
        std::cout << point.x << " " << point.y << std::endl;
    }
}
