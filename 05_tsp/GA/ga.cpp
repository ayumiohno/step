#include "framework.hpp"
#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <queue>
#include <random>
#include <set>
#include <thread>
#include <vector>

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_int_distribution<int> distr(1, 1000000000);

GeneticAlgorithm::GeneticAlgorithm(std::vector<Point>& points, int N_p1, int N_p2, int cycle, int mutation_rate)
{
    this->points = points;
    param.N_p1 = N_p1;
    param.N_p2 = N_p2;
    param.cycle = cycle;
    param.mutation_rate = mutation_rate;
    this->num_of_city = points.size();
}

void GeneticAlgorithm::init()
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

    fitness_ave = 0;
    best_chromosome = unit_now.at(0);
    for (auto& chromosome : unit_now) {
        chromosome.calcTotalDistance(points);
        chromosome.calcFitness(num_of_city);
        fitness_ave += chromosome.fitness;
        if (chromosome.total_distance < best_chromosome.total_distance) {
            this->best_chromosome = chromosome;
        }
    }
    best_chromosome.calcTotalDistance(points);
    fitness_ave = fitness_ave / param.N_p1;
}

void GeneticAlgorithm::optimizePartly(int part_index, int parts_num)
{
    for (int i = unit_next.size() * part_index / parts_num; i < unit_next.size() * (part_index + 1) / parts_num; i++) {
        unit_next.at(i).calcTotalDistance(points);
        //unit_next.at(i).optimize(points, num_of_city);
        unit_next.at(i).calcFitness(num_of_city);
    };
}

void GeneticAlgorithm::optimizeUnitNext()
{
    optimizePartly(0, 1);
    /*auto optimizePartly0 = [&]() { optimizePartly(0, 6); };
    auto optimizePartly1 = [&]() { optimizePartly(1, 6); };
    auto optimizePartly2 = [&]() { optimizePartly(2, 6); };
    auto optimizePartly3 = [&]() { optimizePartly(3, 6); };
    auto optimizePartly4 = [&]() { optimizePartly(4, 6); };
    auto optimizePartly5 = [&]() { optimizePartly(5, 6); };

    std::thread th0(optimizePartly0);
    std::thread th1(optimizePartly1);
    std::thread th2(optimizePartly2);
    std::thread th3(optimizePartly3);
    std::thread th4(optimizePartly4);
    std::thread th5(optimizePartly5);

    th0.join();
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();*/
}

void GeneticAlgorithm::selection()
{
    optimizeUnitNext();
    unit_next.push_back(best_chromosome);
    auto comp = [](const auto& a, const auto& b) { return a.total_distance < b.total_distance; };
    std::priority_queue<Chromosome, std::vector<Chromosome>, decltype(comp)> bestNp1;
    //std::set<double> used;
    assert(unit_next.size() >= param.N_p1);

    for (auto& chromo : unit_next) {
        double distance = chromo.total_distance;
        /*if (used.count(distance)) {
            continue;
        } else {
            used.insert(distance);
        }*/
        if (bestNp1.size() < param.N_p1) {
            bestNp1.push(chromo);
        } else if (distance < bestNp1.top().total_distance) {
            bestNp1.pop();
            bestNp1.push(chromo);
        }
    }
    assert(bestNp1.size() == param.N_p1);

    unit_now.clear();
    fitness_ave = 0;

    while (!bestNp1.empty()) {
        unit_now.push_back(bestNp1.top());
        fitness_ave += bestNp1.top().fitness;
        bestNp1.pop();
    }

    best_chromosome = unit_now.back();
    fitness_ave = fitness_ave / param.N_p1;
}

void GeneticAlgorithm::edgeExchange(int parent1, int parent2)
{
    assert(parent1 < unit_next.size());
    assert(parent2 < unit_next.size());
    int locus1_s = distr(eng) % num_of_city;
    int locus2_s = unit_next.at(parent2).getLocusByFirstCodon(unit_next.at(parent1).chromosome.at(locus1_s).codon1, num_of_city);
    assert(unit_next.at(parent1).chromosome.at(locus1_s).codon1 == unit_next.at(parent2).chromosome.at(locus2_s).codon1);
    while (true) {
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

void GeneticAlgorithm::createNextUnit()
{
    unit_next.clear();
    for (auto& chromo : unit_now) {
        int num_of_child = param.N_p2 / param.N_p1 * chromo.fitness / fitness_ave;
        for (int i = 0; i < num_of_child; i++) {
            unit_next.push_back(chromo);
        }
    }
}

void GeneticAlgorithm::crossOverNextUnit()
{
    //edgeeExchange and selection
    //mutation
    std::shuffle(unit_next.begin(), unit_next.end(), eng);
    int mutation_num = (int)unit_next.size() / param.mutation_rate;
    for (int i = mutation_num; 2 * i + 1 < unit_next.size(); ++i) {
        edgeExchange(2 * i, 2 * i + 1);
    }

    for (int i = 0; i < mutation_num; ++i) {
        for (int j = 0; j < 5; ++j) {
            int rand1 = distr(eng) % num_of_city;
            int rand2 = distr(eng) % num_of_city;
            unit_next.at(i).mutation(rand1, rand2, num_of_city);  //random
        }
    }
}

double GeneticAlgorithm::evolution()
{
    std::cerr << "init" << std::endl;
    init();
    for (int i = 0; i < param.cycle; ++i) {
        std::cerr << "create next" << std::endl;
        createNextUnit();
        std::cerr << "cross over" << std::endl;
        crossOverNextUnit();
        std::cerr << "selection" << std::endl;
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

        /*count = 0;
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
        }*/
    }
    return best_distance;
}

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
