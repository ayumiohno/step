#pragma once
#include "point.hpp"
#include <vector>

struct MacroGene {

    MacroGene() {}
    ~MacroGene() {}

    void inverse();

    int codon1;
    int codon2;
};

struct Chromosome {

    Chromosome() {}
    ~Chromosome() {}

    Chromosome(int num_of_city, std::vector<int>& order);

    void calcTotalDistance(const std::vector<Point>& points);

    int edgeReverse(int start, int target_codon, int num_of_city);

    int getLocusByFirstCodon(int target, int num_of_city);

    void mutation(int start, int end, int num_of_city);

    void reversePath(int start, int end, int num_of_city);

    bool two_opt(int swap1, int swap2, std::vector<Point> points, int num_of_city);

    void optimize(std::vector<Point>& points, int num_of_city);

    std::vector<MacroGene> chromosome;
    double total_distance;
};


struct GeneticAlgorithm {

    double evolution();

    struct Parem {
        int N_p1;
        int N_p2;
        int cycle;
        int mutation_rate;
    } param;

    GeneticAlgorithm(std::vector<Point>& points, int N_p1, int N_p2, int cycle, int mutation_rate);

    void init();

    void optimizePartly(int part_index, int parts_num);

    void optimizeUnitNext();

    void selection();

    void edgeExchange(int parent1, int parent2);

    void createNextUnit();

    void crossOverNextUnit();

    std::vector<Chromosome> unit_next;
    std::vector<Chromosome> unit_now;
    Chromosome best_chromosome;
    double distance_ave;

    std::vector<Point> points;
    int num_of_city;
};
