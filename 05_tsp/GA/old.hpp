#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <queue>
#include <random>
#include <set>
#include <system_error>
#include <thread>
#include <vector>

struct MacroGene {

    MacroGene() {}
    ~MacroGene() {}

    int codon1;
    int codon2;

    void inverse()
    {
        std::swap(this->codon1, this->codon2);
    }
};

struct Chromosome {

    Chromosome() {}
    ~Chromosome() {}

    Chromosome(int num_of_city, std::vector<int>& order)
    {
        this->chromosome.resize(num_of_city);
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

    int edgeReverse(int start, int target_codon, int num_of_city)
    {
        auto locus = (start + 1) % num_of_city;
        while (chromosome.at(locus).codon1 != target_codon) {
            chromosome.at(locus).inverse();
            locus = (locus + 1) % num_of_city;
        }
        //reverse(maybe ok)
        int sum = start + (start < locus ? locus : locus + num_of_city);
        assert(start <= sum - locus);
        for (int i = start + 1; i <= sum / 2; i++) {
            std::swap(chromosome.at(i % num_of_city), chromosome.at((sum - i) % num_of_city));
        }
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

    void mutation(int start, int end)
    {
        if (start + 1 > end) {
            return;
        }
        for (int i = start + 1; i < end; i++) {
            chromosome.at(i).inverse();
        }
        std::reverse(chromosome.begin() + start + 1, chromosome.begin() + end);
    }

    bool two_opt(int swap1, int swap2, std::vector<Point> points, int num_of_city)
    {
        double length_diff = 0;  //入れ替えた後の長さ合計 - 入れ替えた後の長さ合計

        //TODO
        length_diff -= getDistance(points, chromosome.at(swap1).codon1, chromosome.at(swap1).codon2, num_of_city);
        length_diff -= getDistance(points, chromosome.at(swap2).codon1, chromosome.at(swap2).codon2, num_of_city);
        length_diff += getDistance(points, chromosome.at(swap1).codon1, chromosome.at(swap2).codon1, num_of_city);
        length_diff += getDistance(points, chromosome.at(swap1).codon2, chromosome.at(swap2).codon2, num_of_city);

        if (length_diff < 0) {  //入れ替えると長さが短くなる場合
            total_distance += length_diff;
            std::swap(chromosome.at(swap1).codon2, chromosome.at(swap2).codon1);
            int sum = swap1 + (swap1 < swap2 ? swap2 : swap2 + num_of_city);
            for (int i = swap1 + 1; i <= sum / 2; i++) {
                chromosome.at(i % num_of_city).inverse();
                if (i != sum - i) {
                    chromosome.at((sum - i) % num_of_city).inverse();
                }
                std::swap(chromosome.at(i % num_of_city), chromosome.at((sum - i) % num_of_city));
            }
            return true;
        }
        return false;
    }

    void optimize(std::vector<Point>& points, int num_of_city)
    {
        //2opt
        bool is_end = false;
        int count = 0;
        while (!is_end && count < 1) {
            for (int a = 1; a < num_of_city; ++a) {
                is_end = true;
                for (int b = a + 1; b < num_of_city; ++b) {
                    ++count;
                    if (two_opt(a, b, points, num_of_city)) {
                        is_end = false;
                    }
                }
            }
        }
    }

    std::vector<MacroGene> chromosome;
    double total_distance;
};

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_int_distribution<int> distr(1, 1000000000);

struct GeneticAlgorithm {

    struct Parem {
        int N_p1;
        int N_p2;
        int cycle;
    } param;

    GeneticAlgorithm(std::vector<Point>& points, int N_p1, int N_p2, int cycle)
    {
        this->points = points;
        param.N_p1 = N_p1;
        param.N_p2 = N_p2;
        param.cycle = cycle;
        this->num_of_city = points.size();
    }

    void init()
    {
        // Chromosomeをrandomに生成
        std::vector<int> order;
        for (int i = 0; i < num_of_city; i++) {
            order.push_back(i);
        }

        for (int i = 0; i < param.N_p1; i++) {
            std::shuffle(order.begin(), order.end(), eng);
            Chromosome chromosome{num_of_city, order};
            unit_now.push_back(std::move(chromosome));
        }

        distance_ave = 0;
        best_chromosome = unit_now.at(0);
        for (auto& chromosome : unit_now) {
            chromosome.calcTotalDistance(points);
            //chromosome.optimize(points, num_of_city);
            distance_ave += chromosome.total_distance;
            if (chromosome.total_distance < best_chromosome.total_distance) {
                best_chromosome = chromosome;
            }
        }
        distance_ave = distance_ave / param.N_p1;
    }

    void optimizePartly(int part_index, int parts_num)
    {
        for (int i = unit_next.size() * part_index / parts_num; i < unit_next.size() * (part_index + 1) / parts_num; i++) {
            unit_next.at(i).calcTotalDistance(points);
            unit_next.at(i).optimize(points, num_of_city);
        };
    }

    void optimizeUnitNext()
    {
        auto optimizePartly0 = [&]() { optimizePartly(0, 4); };
        auto optimizePartly1 = [&]() { optimizePartly(1, 4); };
        auto optimizePartly2 = [&]() { optimizePartly(2, 4); };
        auto optimizePartly3 = [&]() { optimizePartly(3, 4); };

        std::thread th0(optimizePartly0);
        std::thread th1(optimizePartly1);
        std::thread th2(optimizePartly2);
        std::thread th3(optimizePartly3);

        th0.join();
        th1.join();
        th2.join();
        th3.join();
    }

    void selection()
    {
        //2つのchildを与えられていいのを選ぶ

        optimizeUnitNext();

        unit_next.push_back(best_chromosome);
        auto comp = [](const auto& a, const auto& b) { return a.total_distance < b.total_distance; };
        std::priority_queue<Chromosome, std::vector<Chromosome>, decltype(comp)> bestNp1;
        std::set<double> used;
        assert(unit_next.size() >= param.N_p1);

        for (auto& chromo : unit_next) {
            double distance = chromo.total_distance;
            if (used.count(distance)) {
                continue;
            } else {
                used.insert(distance);
            }
            if (bestNp1.size() < param.N_p1) {
                bestNp1.push(chromo);
            } else if (distance < bestNp1.top().total_distance) {
                bestNp1.pop();
                bestNp1.push(chromo);
            }
        }
        assert(bestNp1.size() == param.N_p1);

        unit_now.clear();
        distance_ave = 0;

        while (!bestNp1.empty()) {
            unit_now.push_back(bestNp1.top());
            distance_ave += bestNp1.top().total_distance;
            bestNp1.pop();
        }

        best_chromosome = unit_now.back();
        distance_ave = distance_ave / param.N_p1;
    }

    void edgeExchange(int parent1, int parent2)
    {
        assert(parent1 < unit_next.size());
        assert(parent2 < unit_next.size());
        int locus1_s = distr(eng) % num_of_city;
        int locus2_s = unit_next.at(parent2).getLocusByFirstCodon(unit_next.at(parent1).chromosome.at(locus1_s).codon1, num_of_city);
        assert(unit_next.at(parent1).chromosome.at(locus1_s).codon1 == unit_next.at(parent2).chromosome.at(locus2_s).codon1);
        while (true) {
            /*debug*****************************************************
            std::cout << locus1_s << " " << locus2_s << std::endl;
            std::cout << "parent1: ";
            for(auto& gene : unit_next.at(parent1).chromosome){
                std::cout << gene.codon1 << " " << gene.codon2 << " : ";
            }
            std::cout << std::endl;
            std::cout << "parent2: ";
            for(auto& gene : unit_next.at(parent2).chromosome){
                std::cout << gene.codon1 << " " << gene.codon2 << " : ";
            }
            std::cout << std::endl;
            ************************************************************/
            int locus1_g = unit_next.at(parent1).edgeReverse(locus1_s, unit_next.at(parent2).chromosome.at(locus2_s).codon2, num_of_city);
            int locus2_g = unit_next.at(parent2).edgeReverse(locus2_s, unit_next.at(parent1).chromosome.at(locus1_s).codon2, num_of_city);
            std::swap(unit_next.at(parent1).chromosome.at(locus1_s), unit_next.at(parent2).chromosome.at(locus2_s));
            if (unit_next.at(parent1).chromosome.at(locus1_s).codon2 == unit_next.at(parent2).chromosome.at(locus2_s).codon2) {
                break;
            }
            locus1_s = locus1_g;
            locus2_s = locus2_g;
        }
    }

    void createNextUnit()
    {
        unit_next.clear();
        for (auto& chromo : unit_now) {
            int num_of_child = param.N_p2 / param.N_p1 * chromo.total_distance / distance_ave;
            for (int i = 0; i < num_of_child; i++) {
                unit_next.push_back(chromo);
            }
        }
    }

    void crossOverNextUnit()
    {
        //edgeeExchange and selection
        //mutation
        std::shuffle(unit_next.begin(), unit_next.end(), eng);
        std::vector<int> mutaions;
        for (int i = 0; i <= unit_next.size() / 2 - 2; ++i) {
            edgeExchange(2 * i, 2 * i + 1);
        }
        int rand1 = distr(eng) % num_of_city;
        int rand2 = distr(eng) % num_of_city;
        //unit_next.back().mutation(std::min(rand1, rand2), std::max(rand1, rand2)); //random
    }

    double evolution()
    {
        init();
        for (int i = 0; i < param.cycle; ++i) {
            createNextUnit();
            crossOverNextUnit();
            selection();
            std::cout << best_chromosome.total_distance << std::endl;
        }
        std::vector<Point> best_points;
        double best_distance = best_chromosome.total_distance;
        for (auto gene : best_chromosome.chromosome) {
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
        std::cerr << is_end << std::endl;
        for (int j = 0; j < 3; ++j) {
            std::cerr << j << std::endl;
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
                            if (reverseSubPath3(a, b, c, best_points, best_distance, num_of_city)) {
                                is_end = false;
                            }
                        }
                    }
                }
            }
        }
        return best_distance;
    }

    std::vector<Chromosome> unit_next;
    std::vector<Chromosome> unit_now;
    Chromosome best_chromosome;
    double distance_ave;

    std::vector<Point> points;
    int num_of_city;
};
