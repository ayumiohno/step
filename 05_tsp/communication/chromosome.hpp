#pragma once
#include "macro_gene.hpp"
#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <vector>

template <int NUM_OF_CITY>
struct Chromosome {

public:
    Chromosome<NUM_OF_CITY>() {}
    ~Chromosome<NUM_OF_CITY>() {}


    Chromosome<NUM_OF_CITY>(int num_of_city, std::vector<int>& order)
    {
        for (int i = 0; i < num_of_city; ++i) {
            this->chromosome.at(i).codon1 = order.at(i);
            this->chromosome.at(i).codon2 = order.at((i + 1) % num_of_city);
        }
    }

    void calcTotalDistance(const std::vector<Point>& points)
    {
        this->total_distance = 0;
        for (auto gene : this->chromosome) {
            this->total_distance += getDistance(points, gene.codon1, gene.codon2, points.size());
        }
    }

    void calcFitness(int num_of_city)
    {
        this->fitness = num_of_city * 100 / total_distance * num_of_city * 100 / total_distance * num_of_city * 100 / total_distance;
    }

private:
    void reversePath(int start, int end, int num_of_city)
    {
        int sum = start + (start < end ? end : end + num_of_city);
        for (int i = start + 1; i <= sum / 2; i++) {
            chromosome.at(i % num_of_city).inverse();
            if (i != sum - i) {
                chromosome.at((sum - i) % num_of_city).inverse();
            }
            std::swap(chromosome.at(i % num_of_city),
                chromosome.at((sum - i) % num_of_city));
        }
    }

public:
    int edgeReverse(int start, int target_codon, int num_of_city)
    {
        auto locus = (start + 1) % num_of_city;
        while (chromosome.at(locus).codon1 != target_codon) {
            locus = (locus + 1) % num_of_city;
        }
        reversePath(start, locus, num_of_city);
        return locus;
    }

    int getLocusByFirstCodon(int target, int num_of_city)
    {
        int locus = 0;
        while (locus < num_of_city && chromosome.at(locus).codon1 != target) {
            ++locus;
        }
        return locus;
    }

    void mutation(int start, int end, int num_of_city)
    {
        if (start == end) {
            return;
        }
        std::swap(chromosome.at(start).codon2, chromosome.at(end).codon1);
        reversePath(start, end, num_of_city);
    }

private:
    bool twoOpt(int swap1, int swap2, std::vector<Point> points,
        int num_of_city)
    {
        double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計

        length_diff -= getDistance(points, chromosome.at(swap1).codon1,
            chromosome.at(swap1).codon2, num_of_city);
        length_diff -= getDistance(points, chromosome.at(swap2).codon1,
            chromosome.at(swap2).codon2, num_of_city);
        length_diff += getDistance(points, chromosome.at(swap1).codon1,
            chromosome.at(swap2).codon1, num_of_city);
        length_diff += getDistance(points, chromosome.at(swap1).codon2,
            chromosome.at(swap2).codon2, num_of_city);

        if (length_diff < 0) {  //入れ替えると長さが短くなる場合
            total_distance += length_diff;
            std::swap(chromosome.at(swap1).codon2, chromosome.at(swap2).codon1);
            reversePath(swap1, swap2, num_of_city);
            return true;
        }
        return false;
    }

    //       ---  target ---  target+1 ---                 ---  target + 1  ---                                     --- target ---
    //            a | b        b | c                               a | c                                                 a | c
    //                                       =>           (insert_pos < target)                                  (insert_pos > target)
    //          ---    insert_pos   ---               ---insert_pos  --- insert_pos + 1 ---            --- insert_pos - 1 --- insert_pos  ---
    //                    d | e                             d | b            b | e                             d | b            b | e
    bool one_move(int target, int insert_pos, std::vector<Point>& points, int num_of_city)
    {
        if (target == insert_pos || (target + 1 - insert_pos) % num_of_city == 0) {
            return false;
        }

        double length_diff = 0;

        length_diff -= getDistance(points, chromosome.at(target).codon1,
            chromosome.at(target).codon2, num_of_city);
        length_diff -= getDistance(
            points, chromosome.at((target + 1) % num_of_city).codon1,
            chromosome.at((target + 1) % num_of_city).codon2, num_of_city);
        length_diff -= getDistance(points, chromosome.at(insert_pos).codon1,
            chromosome.at(insert_pos).codon2, num_of_city);
        length_diff += getDistance(points, chromosome.at(target).codon1,
            chromosome.at((target + 1) % num_of_city).codon2,
            num_of_city);
        length_diff += getDistance(points, chromosome.at(target).codon2,
            chromosome.at(insert_pos).codon1, num_of_city);
        length_diff += getDistance(points, chromosome.at(target).codon2,
            chromosome.at(insert_pos).codon2, num_of_city);

        if (length_diff < 0) {  //入れ替えると長さが短くなる場合
            total_distance += length_diff;

            chromosome.at((target + 1) % num_of_city).codon1 = chromosome.at(target).codon1;
            chromosome.at(target).codon1 = chromosome.at(insert_pos).codon1;
            chromosome.at(insert_pos).codon1 = chromosome.at(target).codon2;


            if (insert_pos < target) {
                for (int i = target; i > insert_pos; i--) {
                    std::swap(chromosome.at(i), chromosome.at(i - 1));
                }
            } else {
                for (int i = target; i < insert_pos - 1; i++) {
                    std::swap(chromosome.at(i), chromosome.at(i + 1));
                }
            }

            return true;
        }
        return false;
    }

    //     target-1 --- target ---  target+1 ---             ---  target  ---                         --- target ---
    //       a | b       b | c       c | d                         a | d                                   a | d
    //                                       =>                                        or
    //          ---   insert_pos   ---                      --- insert_pos ---                      ---  insert_pos  ---
    //                   d | e                          d | b      b | c     c | e                d | c    c | b    b | e

    bool two_move(int target, int insert_pos, std::vector<Point>& points, int num_of_city)
    {
        if (target == insert_pos || (target + 1 - insert_pos) % num_of_city == 0 || (target - 1 - insert_pos) % num_of_city == 0) {
            return false;
        }

        double length_diff = 0;
        double length_diff1 = 0;
        double length_diff2 = 0;

        length_diff -= getDistance(points, chromosome.at((target - 1 + num_of_city) % num_of_city).codon1, chromosome.at((target - 1 + num_of_city) % num_of_city).codon2, num_of_city);
        length_diff -= getDistance(points, chromosome.at((target + 1) % num_of_city).codon1, chromosome.at((target + 1) % num_of_city).codon2, num_of_city);
        length_diff -= getDistance(points, chromosome.at(insert_pos).codon1, chromosome.at(insert_pos).codon2, num_of_city);
        length_diff += getDistance(points, chromosome.at((target - 1 + num_of_city) % num_of_city).codon1, chromosome.at((target + 1) % num_of_city).codon2, num_of_city);

        length_diff1 += getDistance(points, chromosome.at(target).codon1, chromosome.at(insert_pos).codon1, num_of_city);
        length_diff1 += getDistance(points, chromosome.at(target).codon2, chromosome.at(insert_pos).codon2, num_of_city);

        length_diff2 += getDistance(points, chromosome.at(target).codon2, chromosome.at(insert_pos).codon1, num_of_city);
        length_diff2 += getDistance(points, chromosome.at(target).codon1, chromosome.at(insert_pos).codon2, num_of_city);


        length_diff += std::min(length_diff1, length_diff2);
        if (length_diff < 0) {  //入れ替えると長さが短くなる場合
            /*std::cout << "target: " << target << " insert " << insert_pos << std::endl;
            std::cout << "before: ";
            for(auto& gene : chromosome){
                std::cout << gene.codon1 << " : " << gene.codon2 << " ";
            }
            std::cout << std::endl;*/

            if (length_diff2 < length_diff1) {
                chromosome.at(target).inverse();
                chromosome.at((target - 1 + num_of_city) % num_of_city).codon2 = chromosome.at(target).codon1;
            } else {
            }
            total_distance += length_diff1;

            chromosome.at((target + 1) % num_of_city).codon1 = chromosome.at((target - 1 + num_of_city) % num_of_city).codon1;
            chromosome.at((target - 1 + num_of_city) % num_of_city).codon1 = chromosome.at(insert_pos).codon1;
            chromosome.at(insert_pos).codon1 = chromosome.at(target).codon2;

            auto tar1 = chromosome.at((target - 1 + num_of_city) % num_of_city);
            auto tar2 = chromosome.at(target);

            if (target == 0) {
                for (int i = 0; i + 1 <= insert_pos - 1; i++) {
                    chromosome.at(i) = chromosome.at(i + 1);
                }
                for (int i = num_of_city - 1; i - 1 <= insert_pos; i--) {
                    chromosome.at(i) = chromosome.at(i - 1);
                }
                chromosome.at(insert_pos - 1) = tar1;
                chromosome.at(insert_pos) = tar2;
            } else if (insert_pos < target) {
                for (int i = target; i - 2 >= insert_pos; i--) {
                    chromosome.at(i) = chromosome.at(i - 2);
                }
                chromosome.at(insert_pos) = tar1;
                chromosome.at(insert_pos + 1) = tar2;
            } else {
                for (int i = target - 1; i + 2 <= insert_pos - 1; i++) {
                    chromosome.at(i) = chromosome.at(i + 2);
                }
                chromosome.at(insert_pos - 2) = tar1;
                chromosome.at(insert_pos - 1) = tar2;
            }

            /*std::cout << "after: ";
            for(auto& gene : chromosome){
                std::cout << gene.codon1 << " : " << gene.codon2 << " ";
            }
            std::cout << std::endl;*/

            return true;
        }
        return false;
    }

public:
    void optimize(std::vector<Point>& points, int num_of_city)
    {
        bool is_end = false;
        int count = 0;
        while (!is_end && count < 5) {
            ++count;
            for (int a = 1; a < num_of_city; ++a) {
                is_end = true;
                for (int b = a + 1; b < num_of_city; ++b) {
                    ++count;
                    if (twoOpt(a, b, points, num_of_city)) {
                        is_end = false;
                    }
                }
            }
        }
        is_end = false;
        count = 0;
        while (!is_end && count < 1) {
            ++count;
            for (int a = 0; a < num_of_city; ++a) {
                is_end = true;
                for (int b = 0; b < num_of_city; ++b) {
                    if (one_move(a, b, points, num_of_city)) {
                        is_end = false;
                    }
                }
            }
        }
        is_end = false;
        count = 0;
        while (!is_end && count < 1) {
            ++count;
            for (int a = 1; a < num_of_city; ++a) {
                is_end = true;
                for (int b = 1; b < num_of_city; ++b) {
                    if (two_move(a, b, points, num_of_city)) {
                        is_end = false;
                    }
                }
            }
        }

        while (!is_end && count < 5) {
            ++count;
            for (int a = 1; a < num_of_city; ++a) {
                is_end = true;
                for (int b = a + 1; b < num_of_city; ++b) {
                    ++count;
                    if (twoOpt(a, b, points, num_of_city)) {
                        is_end = false;
                    }
                }
            }
        }
    }

    double optimizeFinaly(std::vector<Point>& points, int num_of_city)
    {
        std::vector<Point> best_points;
        double best_distance = total_distance;
        for (auto gene : chromosome) {
            best_points.push_back(points.at(gene.codon1));
        }
        bool is_end = false;
        int count = 0;
        while (!is_end && count < 10000000) {
            for (int a = 1; a < num_of_city; ++a) {
                is_end = true;
                for (int b = a + 1; b < num_of_city; ++b) {
                    ++count;
                    if (reverseSubPath(a, b, best_points, best_distance, num_of_city)) {
                        is_end = false;
                    }
                }
            }
        }
        for (int j = 0; j < 3; ++j) {
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for (int base = 0; base < num_of_city; ++base) {
                    ++count;
                    if (movePoint(base, best_points, best_distance, num_of_city)) {
                        is_end = false;
                    }
                }
            }

            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for (int base = 1; base < num_of_city; ++base) {
                    ++count;
                    if (move2Points(base, best_points, best_distance, num_of_city)) {
                        is_end = false;
                    }
                }
            }
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for (int base = 1; base < num_of_city; ++base) {
                    ++count;
                    if (move3Points(base, best_points, best_distance, num_of_city)) {
                        is_end = false;
                    }
                }
            }
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for (int base = 1; base < num_of_city; ++base) {
                    ++count;
                    if (move4Points(base, best_points, best_distance, num_of_city)) {
                        is_end = false;
                    }
                }
            }
            count = 0;
            is_end = false;
            while (!is_end && count < 10000) {
                is_end = true;
                for (int a = 1; a < num_of_city; ++a) {
                    for (int b = a + 1; b < num_of_city; ++b) {
                        ++count;
                        if (reverseSubPath(a, b, best_points, best_distance, num_of_city)) {
                            is_end = false;
                        }
                    }
                }
            }

            count = 0;
            is_end = false;
            while (!is_end && count < 1) {
                is_end = true;
                for (int a = 0; a < num_of_city; ++a) {
                    for (int b = a + 1; b < num_of_city; ++b) {
                        for (int c = b + 1; c < num_of_city; ++c) {
                            ++count;
                            if (reverseSubPath3(a, b, c, best_points, best_distance,
                                    num_of_city)) {
                                is_end = false;
                            }
                        }
                    }
                }
            }
        }
        return best_distance;
    }

    std::array<MacroGene, NUM_OF_CITY> chromosome;
    double total_distance;
    double fitness;
};
