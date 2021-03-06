#include "framework.hpp"
#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <vector>

Chromosome::Chromosome(int num_of_city, std::vector<int>& order)
{
    this->chromosome.resize(num_of_city);
    for (int i = 0; i < num_of_city; ++i) {
        this->chromosome.at(i).codon1 = order.at(i);
        this->chromosome.at(i).codon2 = order.at((i + 1) % num_of_city);
    }
}

void Chromosome::calcTotalDistance(const std::vector<Point>& points)
{
    this->total_distance = 0;
    for (auto gene : this->chromosome) {
        this->total_distance += getDistance(points, gene.codon1, gene.codon2, points.size());
    }
}

void Chromosome::calcFitness(int num_of_city)
{
    this->fitness = num_of_city * 100 / total_distance * num_of_city * 100 / total_distance * num_of_city * 100 / total_distance;
}


int Chromosome::edgeReverse(int start, int target_codon, int num_of_city)
{
    auto locus = (start + 1) % num_of_city;
    while (chromosome.at(locus).codon1 != target_codon) {
        locus = (locus + 1) % num_of_city;
    }
    reversePath(start, locus, num_of_city);
    return locus;
}

int Chromosome::getLocusByFirstCodon(int target, int num_of_city)
{
    int locus = 0;
    while (locus < num_of_city && chromosome.at(locus).codon1 != target) {
        ++locus;
    }
    return locus;
}

void Chromosome::mutation(int start, int end, int num_of_city)
{
    if (start == end) {
        return;
    }
    std::swap(chromosome.at(start).codon2, chromosome.at(end).codon1);
    reversePath(start, end, num_of_city);
}

void Chromosome::reversePath(int start, int end, int num_of_city)
{
    int sum = start + (start < end ? end : end + num_of_city);
    for (int i = start + 1; i <= sum / 2; i++) {
        chromosome.at(i % num_of_city).inverse();
        if (i != sum - i) {
            chromosome.at((sum - i) % num_of_city).inverse();
        }
        std::swap(chromosome.at(i % num_of_city), chromosome.at((sum - i) % num_of_city));
    }
}

void Chromosome::applyOptimize2D(std::function<bool(int, int, std::vector<Point>&, int)>& func, std::vector<Point>& points, int num_of_city)
{
    bool is_end = false;
    int count = 0;
    while (!is_end && count < 1) {
        for (int a = 1; a < num_of_city; ++a) {
            is_end = true;
            for (int b = a + 1; b < num_of_city; ++b) {
                ++count;
                if (func(a, b, points, num_of_city)) {
                    is_end = false;
                }
            }
        }
    }
}

void Chromosome::optimize(std::vector<Point>& points, int num_of_city)
{
    //2opt
    //auto two_opt = [&](int a, int b, std::vector<Point>& points, int num_of_city){return twoOpt(a, b, points, num_of_city);};
    //applyOptimize2D(two_opt, points, num_of_city);
    bool is_end = false;
    int count = 0;
    while (!is_end && count < 1) {
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
        for (int a = 1; a < num_of_city; ++a) {
            is_end = true;
            for (int b = a + 1; b < num_of_city; ++b) {
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
            for (int b = a + 1; b < num_of_city; ++b) {
                if (two_move(a, b, points, num_of_city)) {
                    is_end = false;
                }
            }
        }
    }
}

bool Chromosome::twoOpt(int swap1, int swap2, std::vector<Point> points, int num_of_city)
{
    double length_diff = 0;  //????????????????????????????????? - ?????????????????????????????????

    length_diff -= getDistance(points, chromosome.at(swap1).codon1, chromosome.at(swap1).codon2, num_of_city);
    length_diff -= getDistance(points, chromosome.at(swap2).codon1, chromosome.at(swap2).codon2, num_of_city);
    length_diff += getDistance(points, chromosome.at(swap1).codon1, chromosome.at(swap2).codon1, num_of_city);
    length_diff += getDistance(points, chromosome.at(swap1).codon2, chromosome.at(swap2).codon2, num_of_city);

    if (length_diff < 0) {  //?????????????????????????????????????????????
        total_distance += length_diff;
        std::swap(chromosome.at(swap1).codon2, chromosome.at(swap2).codon1);
        reversePath(swap1, swap2, num_of_city);
        return true;
    }
    return false;
}

bool Chromosome::one_move(int target, int insert_pos, std::vector<Point>& points, int num_of_city)
{
    if (target == insert_pos || (target + 1 - insert_pos) % num_of_city == 0) {
        return false;
    }

    double length_diff = 0;

    length_diff -= getDistance(points, chromosome.at(target).codon1, chromosome.at(target).codon2, num_of_city);
    length_diff -= getDistance(points, chromosome.at((target + 1) % num_of_city).codon1, chromosome.at((target + 1) % num_of_city).codon2, num_of_city);
    length_diff -= getDistance(points, chromosome.at(insert_pos).codon1, chromosome.at(insert_pos).codon2, num_of_city);
    length_diff += getDistance(points, chromosome.at(target).codon1, chromosome.at((target + 1) % num_of_city).codon2, num_of_city);
    length_diff += getDistance(points, chromosome.at(target).codon2, chromosome.at(insert_pos).codon1, num_of_city);
    length_diff += getDistance(points, chromosome.at(target).codon2, chromosome.at(insert_pos).codon2, num_of_city);

    if (length_diff < 0) {  //?????????????????????????????????????????????
        total_distance += length_diff;
        chromosome.at((target + 1) % num_of_city).codon1 = chromosome.at(target).codon1;
        chromosome.at(target).codon1 = chromosome.at(insert_pos).codon1;
        chromosome.at(insert_pos).codon1 = chromosome.at(target).codon2;

        chromosome.insert(chromosome.begin() + insert_pos, chromosome.at(target));
        int erase_pos = target < insert_pos ? target : target + 1;
        chromosome.erase(chromosome.begin() + erase_pos);
        return true;
    }
    return false;
}

bool Chromosome::two_move(int target, int insert_pos, std::vector<Point>& points, int num_of_city)
{
    if (target == insert_pos || (target + 1 - insert_pos) % num_of_city == 0 || (target - 1 - insert_pos) % num_of_city == 0) {
        return false;
    }

    double length_diff = 0;

    length_diff -= getDistance(points, chromosome.at((target - 1 + num_of_city) % num_of_city).codon1, chromosome.at((target - 1 + num_of_city) % num_of_city).codon2, num_of_city);
    length_diff -= getDistance(points, chromosome.at((target + 1) % num_of_city).codon1, chromosome.at((target + 1) % num_of_city).codon2, num_of_city);
    length_diff -= getDistance(points, chromosome.at(insert_pos).codon1, chromosome.at(insert_pos).codon2, num_of_city);
    length_diff += getDistance(points, chromosome.at((target - 1 + num_of_city) % num_of_city).codon1, chromosome.at((target + 1) % num_of_city).codon2, num_of_city);
    length_diff += getDistance(points, chromosome.at(target).codon1, chromosome.at(insert_pos).codon1, num_of_city);
    length_diff += getDistance(points, chromosome.at(target).codon2, chromosome.at(insert_pos).codon2, num_of_city);

    if (length_diff < 0) {  //?????????????????????????????????????????????
        total_distance += length_diff;
        chromosome.at((target + 1) % num_of_city).codon1 = chromosome.at((target - 1 + num_of_city) % num_of_city).codon1;
        chromosome.at((target - 1 + num_of_city) % num_of_city).codon1 = chromosome.at(insert_pos).codon1;
        chromosome.at(insert_pos).codon1 = chromosome.at(target).codon2;

        if (target != 0) {
            chromosome.insert(chromosome.begin() + insert_pos, chromosome.at(target));
            chromosome.insert(chromosome.begin() + insert_pos, chromosome.at(target - 1));
            int erase_pos = target < insert_pos ? target : target + 2;
            chromosome.erase(chromosome.begin() + erase_pos);
            chromosome.erase(chromosome.begin() + erase_pos - 1);
        } else {
            chromosome.insert(chromosome.begin() + insert_pos, chromosome.at(num_of_city - 1));
            chromosome.insert(chromosome.begin() + insert_pos, chromosome.at(0));
            chromosome.erase(chromosome.begin() + 0);
            chromosome.erase(chromosome.begin() + num_of_city);
        }
        return true;
    }
    return false;
}
