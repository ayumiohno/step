#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <iostream>
#include <random>
#include <set>
#include <utility>
#include <vector>

double getDistance(const std::vector<Point>& points, int i, int j, int num_of_dots)
{
    auto& p1 = points.at((i + num_of_dots) % num_of_dots);
    auto& p2 = points.at((j + num_of_dots) % num_of_dots);
    return std::sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

// p1-1        p1       p1-1        p1
//   * -  -  - *         *  -     -  *
//             |    =>         -     |
//   * -  -  - *         *  -     -  *
// p2+1        p2       p2+1        p2
bool twoOpt(const int& swap1, const int& swap2,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots)
{
    auto swap_pair = std::minmax(swap1, swap2);
    if (swap_pair.second == swap_pair.first || swap_pair.first <= 0 || swap_pair.second >= num_of_dots - 1) {
        return false;
    }
    double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計

    length_diff -= getDistance(points, swap_pair.first - 1, swap_pair.first, num_of_dots);
    length_diff -= getDistance(points, swap_pair.second, swap_pair.second + 1, num_of_dots);
    length_diff += getDistance(points, swap_pair.first - 1, swap_pair.second, num_of_dots);
    length_diff += getDistance(points, swap_pair.first, swap_pair.second + 1, num_of_dots);

    if (length_diff < 0) {  //入れ替えると長さが短くなる場合
        min_length += length_diff;
        std::reverse(points.begin() + swap_pair.first, points.begin() + swap_pair.second + 1);
        return true;
    }
    return false;
}

//   ---base - 1 -            - base + 1  ---            --- base - 1      ---     base + 1   ---
//                 -         -
//                   - base -                     =>                     - base -
//                                                                      -        -
//   ---  i - 1        ---         i      ---            ---   i - 1   -          -  i        ---

bool movePoint(const int& base,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots)
{
    double min_length_diff = 0;
    int i_save;
    for (int i = 0; i < num_of_dots; ++i) {
        double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計
        if (base == (i - 1 + num_of_dots) % num_of_dots || base == i) {
            continue;
        }
        length_diff += getDistance(points, base, i - 1, num_of_dots);
        length_diff += getDistance(points, base, i, num_of_dots);
        length_diff += getDistance(points, base - 1, base + 1, num_of_dots);
        length_diff -= getDistance(points, base, base - 1, num_of_dots);
        length_diff -= getDistance(points, base, base + 1, num_of_dots);
        length_diff -= getDistance(points, i - 1, i, num_of_dots);

        if (length_diff < min_length_diff) {
            i_save = i;
            min_length_diff = length_diff;
        }
    }
    if (min_length_diff < 0) {
        min_length += min_length_diff;
        auto base_point = points.at(base);
        points.erase(points.begin() + base);
        assert(points.size() == num_of_dots - 1);
        int new_i = base < i_save ? i_save - 1 : i_save;
        points.insert(points.begin() + new_i, base_point);
        assert(points.size() == num_of_dots);
        return true;
    }
    return false;
}

//   ---base - 1 -                    - base + 2---     --- base - 1         ---           base + 2 ---       --- base - 1      ---      base + 2 ---
//               -                    -
//               - base --- base + 1  -              =>               - base --- base + 1 -                or           - base - base + 1 -
//                                                                    -                   -                             -                 -
//   ---  i - 1         ---                 i   ---     ---   i - 1   -                   -    i    ---       ---   i   -                 - i - 1 ---
bool move2Points(const int& base,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots)
{
    double min_length_diff = 0;
    int i_save;
    bool dir;
    for (int i = 0; i < num_of_dots; ++i) {
        double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計
        double length_diff1 = 0;
        double length_diff2 = 0;
        if (base == i || base == (i - 1 + num_of_dots) % num_of_dots || (base + 1) % num_of_dots == (i - 1 + num_of_dots) % num_of_dots) {
            continue;
        }

        length_diff -= getDistance(points, base - 1, base, num_of_dots);
        length_diff -= getDistance(points, base + 1, base + 2, num_of_dots);
        length_diff += getDistance(points, base - 1, base + 2, num_of_dots);
        length_diff -= getDistance(points, i - 1, i, num_of_dots);

        length_diff1 += getDistance(points, base, i - 1, num_of_dots);
        length_diff1 += getDistance(points, base + 1, i, num_of_dots);
        length_diff2 += getDistance(points, base, i, num_of_dots);
        length_diff2 += getDistance(points, base + 1, i - 1, num_of_dots);

        if (length_diff1 < length_diff2) {
            length_diff += length_diff1;
            if (length_diff < min_length_diff) {
                i_save = i;
                min_length_diff = length_diff;
                dir = true;
            }
        } else {
            length_diff += length_diff2;
            if (length_diff < min_length_diff) {
                i_save = i;
                min_length_diff = length_diff;
                dir = false;
            }
        }
    }
    if (min_length_diff < 0) {
        min_length += min_length_diff;
        auto base_point1 = points.at(base);
        auto base_point2 = points.at((base + 1) % num_of_dots);
        int new_i;
        if (base + 1 != num_of_dots) {
            points.erase(points.begin() + base + 1);
            points.erase(points.begin() + base);
            new_i = base < i_save ? i_save - 2 : i_save;
        } else {
            points.erase(points.begin() + num_of_dots - 1);
            points.erase(points.begin());
            new_i = i_save - 1;
        }
        if (dir) {
            points.insert(points.begin() + new_i, base_point2);
            points.insert(points.begin() + new_i, base_point1);
        } else {
            points.insert(points.begin() + new_i, base_point1);
            points.insert(points.begin() + new_i, base_point2);
        }
        return true;
    }
    return false;
}

//   ---base - 1 -                    - base + 3---     --- base - 1         ---           base + 2 ---       --- base - 1      ---      base + 2 ---
//               -                    -
//               - b  - b + 1 - b + 2  -              =>               - base --- base + 1 -                or           - base - base + 1 -
//                                                                    -                   -                             -                 -
//   ---  i - 1         ---                 i   ---     ---   i - 1   -                   -    i    ---       ---   i   -                 - i - 1 ---
bool move3Points(const int& base,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots)
{
    double min_length_diff = 0;
    int i_save;
    bool dir;
    for (int i = 0; i < num_of_dots; ++i) {
        double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計
        double length_diff1 = 0;
        double length_diff2 = 0;
        if (base == i || (base - i + 1) % num_of_dots == 0 || (base + 1 - i + 1) % num_of_dots == 0 || (base + 2 - i + 1) % num_of_dots == 0) {
            continue;
        }

        length_diff -= getDistance(points, base - 1, base, num_of_dots);
        length_diff -= getDistance(points, base + 2, base + 3, num_of_dots);
        length_diff -= getDistance(points, i - 1, i, num_of_dots);
        length_diff += getDistance(points, base - 1, base + 3, num_of_dots);

        length_diff1 += getDistance(points, base, i - 1, num_of_dots);
        length_diff1 += getDistance(points, base + 2, i, num_of_dots);
        length_diff2 += getDistance(points, base, i, num_of_dots);
        length_diff2 += getDistance(points, base + 2, i - 1, num_of_dots);

        if (length_diff1 < length_diff2) {
            length_diff += length_diff1;
            if (length_diff < min_length_diff) {
                i_save = i;
                min_length_diff = length_diff;
                dir = true;
            }
        } else {
            length_diff += length_diff2;
            if (length_diff < min_length_diff) {
                i_save = i;
                min_length_diff = length_diff;
                dir = false;
            }
        }
    }
    if (min_length_diff < 0) {
        min_length += min_length_diff;
        std::cerr << "3-move " << min_length << std::endl;
        auto base_point1 = points.at(base);
        auto base_point2 = points.at((base + 1) % num_of_dots);
        auto base_point3 = points.at((base + 2) % num_of_dots);
        int new_i;
        if (base + 1 == num_of_dots) {
            points.erase(points.begin() + num_of_dots - 1);
            points.erase(points.begin());
            points.erase(points.begin() + 1);
            new_i = i_save - 2;
        } else if (base + 2 == num_of_dots) {
            points.erase(points.begin() + num_of_dots - 2);
            points.erase(points.begin() + num_of_dots - 1);
            points.erase(points.begin());
            new_i = i_save - 1;
        } else {
            points.erase(points.begin() + base + 2);
            points.erase(points.begin() + base + 1);
            points.erase(points.begin() + base);
            new_i = base < i_save ? i_save - 3 : i_save;
        }
        if (dir) {
            points.insert(points.begin() + new_i, base_point3);
            points.insert(points.begin() + new_i, base_point2);
            points.insert(points.begin() + new_i, base_point1);
        } else {
            points.insert(points.begin() + new_i, base_point1);
            points.insert(points.begin() + new_i, base_point2);
            points.insert(points.begin() + new_i, base_point3);
        }
        return true;
    }
    return false;
}

bool move4Points(const int& base,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots)
{
    double min_length_diff = 0;
    int i_save;
    bool dir;
    for (int i = 0; i < num_of_dots; ++i) {
        double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計
        double length_diff1 = 0;
        double length_diff2 = 0;
        if (base == i || (base - i + 1) % num_of_dots == 0 || (base + 1 - i + 1) % num_of_dots == 0
            || (base + 2 - i + 1) % num_of_dots == 0 || (base + 3 - i + 1) % num_of_dots == 0) {
            continue;
        }

        length_diff -= getDistance(points, base - 1, base, num_of_dots);
        length_diff -= getDistance(points, base + 3, base + 4, num_of_dots);
        length_diff -= getDistance(points, i - 1, i, num_of_dots);
        length_diff += getDistance(points, base - 1, base + 4, num_of_dots);

        length_diff1 += getDistance(points, base, i - 1, num_of_dots);
        length_diff1 += getDistance(points, base + 3, i, num_of_dots);
        length_diff2 += getDistance(points, base, i, num_of_dots);
        length_diff2 += getDistance(points, base + 3, i - 1, num_of_dots);

        if (length_diff1 < length_diff2) {
            length_diff += length_diff1;
            if (length_diff < min_length_diff) {
                i_save = i;
                min_length_diff = length_diff;
                dir = true;
            }
        } else {
            length_diff += length_diff2;
            if (length_diff < min_length_diff) {
                i_save = i;
                min_length_diff = length_diff;
                dir = false;
            }
        }
    }
    if (min_length_diff < 0) {
        min_length += min_length_diff;
        std::cerr << "4-move " << min_length << std::endl;
        auto base_point1 = points.at(base);
        auto base_point2 = points.at((base + 1) % num_of_dots);
        auto base_point3 = points.at((base + 2) % num_of_dots);
        auto base_point4 = points.at((base + 3) % num_of_dots);
        int new_i;
        if (base + 1 == num_of_dots) {
            points.erase(points.begin() + num_of_dots - 1);
            points.erase(points.begin());
            points.erase(points.begin() + 1);
            points.erase(points.begin() + 2);
            new_i = i_save - 3;
        } else if (base + 2 == num_of_dots) {
            points.erase(points.begin() + num_of_dots - 2);
            points.erase(points.begin() + num_of_dots - 1);
            points.erase(points.begin());
            points.erase(points.begin() + 1);
            new_i = i_save - 2;
        } else if (base + 3 == num_of_dots) {
            points.erase(points.begin() + num_of_dots - 3);
            points.erase(points.begin() + num_of_dots - 2);
            points.erase(points.begin() + num_of_dots - 1);
            points.erase(points.begin());
            new_i = i_save - 1;
        } else {
            points.erase(points.begin() + base + 3);
            points.erase(points.begin() + base + 2);
            points.erase(points.begin() + base + 1);
            points.erase(points.begin() + base);
            new_i = base < i_save ? i_save - 4 : i_save;
        }
        if (dir) {
            points.insert(points.begin() + new_i, base_point4);
            points.insert(points.begin() + new_i, base_point3);
            points.insert(points.begin() + new_i, base_point2);
            points.insert(points.begin() + new_i, base_point1);
        } else {
            points.insert(points.begin() + new_i, base_point1);
            points.insert(points.begin() + new_i, base_point2);
            points.insert(points.begin() + new_i, base_point3);
            points.insert(points.begin() + new_i, base_point4);
        }
        return true;
    }
    return false;
}

bool threeOpt(const int& swap1, const int& swap2, const int& swap3,
    std::vector<Point>& points,
    double& min_length, const int& num_of_dots)
{
    if (swap1 + 1 == swap2 || swap2 + 1 == swap3 || (swap3 + 1) % num_of_dots == swap1) {
        return false;
    }

    double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計


    double length_diff1 = 0;
    double length_diff2 = 0;
    double length_diff3 = 0;
    double length_diff4 = 0;

    int dir = 1;

    length_diff1 += getDistance(points, swap1, swap3 - 1, num_of_dots);
    length_diff1 += getDistance(points, swap2 - 1, swap1 - 1, num_of_dots);
    length_diff1 += getDistance(points, swap2, swap3, num_of_dots);

    length_diff = length_diff1;

    length_diff2 += getDistance(points, swap1, swap2, num_of_dots);
    length_diff2 += getDistance(points, swap2 - 1, swap3, num_of_dots);
    length_diff2 += getDistance(points, swap3 - 1, swap1 - 1, num_of_dots);

    if (length_diff2 < length_diff) {
        dir = 2;
        length_diff = length_diff2;
    }

    length_diff3 += getDistance(points, swap1, swap3, num_of_dots);
    length_diff3 += getDistance(points, swap2 - 1, swap3 - 1, num_of_dots);
    length_diff3 += getDistance(points, swap2, swap1 - 1, num_of_dots);

    if (length_diff3 < length_diff) {
        dir = 3;
        length_diff = length_diff3;
    }

    length_diff4 += getDistance(points, swap1, swap3 - 1, num_of_dots);
    length_diff4 += getDistance(points, swap2, swap1 - 1, num_of_dots);
    length_diff4 += getDistance(points, swap3, swap2 - 1, num_of_dots);

    if (length_diff4 < length_diff) {
        dir = 4;
        length_diff = length_diff4;
    }

    length_diff -= getDistance(points, swap1 - 1, swap1, num_of_dots);
    length_diff -= getDistance(points, swap2 - 1, swap2, num_of_dots);
    length_diff -= getDistance(points, swap3 - 1, swap3, num_of_dots);

    if (length_diff < 0) {  //入れ替えると長さが短くなる場合
        min_length += length_diff;
        std::cerr << "3-opt " << min_length << std::endl;
        std::vector<Point> new_vec;
        if (dir == 1) {
            for (int i = 0; i <= swap1 - 1; ++i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap2 - 1; i >= swap1; --i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap3 - 1; i >= swap2; --i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap3; i < num_of_dots; ++i) {
                new_vec.push_back(points.at(i));
            }
        } else if (dir == 2) {
            for (int i = 0; i <= swap1 - 1; ++i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap3 - 1; i >= swap2; --i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap1; i <= swap2 - 1; ++i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap3; i < num_of_dots; ++i) {
                new_vec.push_back(points.at(i));
            }
        } else if (dir == 3) {
            for (int i = 0; i <= swap1 - 1; ++i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap2; i <= swap3 - 1; ++i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap2 - 1; i >= swap1; --i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap3; i < num_of_dots; ++i) {
                new_vec.push_back(points.at(i));
            }
        } else if (dir == 4) {
            for (int i = 0; i <= swap1 - 1; ++i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap2; i <= swap3 - 1; ++i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap1; i <= swap2 - 1; ++i) {
                new_vec.push_back(points.at(i));
            }
            for (int i = swap3; i < num_of_dots; ++i) {
                new_vec.push_back(points.at(i));
            }
        }
        assert(new_vec.size() == num_of_dots);
        points = std::move(new_vec);
        return true;
    }
    return false;
}
