#pragma once
#include "chromosome.hpp"
#include "macro_gene.hpp"
#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <arpa/inet.h>  //バイトオーダの変換に利用
#include <array>
#include <assert.h>
#include <cstring>
#include <iostream>
#include <queue>
#include <random>
#include <set>
#include <stdio.h>
#include <sys/socket.h>  //アドレスドメイン
#include <sys/types.h>   //ソケットタイプ
#include <thread>
#include <unistd.h>  //close()に利用
#include <vector>

std::random_device rd;
int seed = rd();
std::default_random_engine eng(seed);
std::uniform_int_distribution<int> distr(1, 1000000000);

constexpr int DIV = 16;

template <int NUM_OF_CITY>
struct GeneticAlgorithm {

    struct Param {
        int N_p1;
        int N_p2;
        int cycle;
        int mutation_rate;
    } param;

    GeneticAlgorithm<NUM_OF_CITY>(std::vector<Point>& points, int N_p1, int N_p2, int cycle, int mutation_rate)
    {
        this->points = points;
        param.N_p1 = N_p1;
        param.N_p2 = N_p2;
        param.cycle = cycle;
        param.mutation_rate = mutation_rate;
        this->num_of_city = points.size();
    }

    void init()
    {
        // Chromosomeをrandomに生成
        std::cout << "seed: " << seed << std::endl;
        std::vector<int> order;
        for (int i = 0; i < num_of_city; i++) {
            order.push_back(i);
        }

        for (int i = 0; i < param.N_p1; i++) {
            std::shuffle(order.begin(), order.end(), eng);
            Chromosome<NUM_OF_CITY> chromosome{num_of_city, order};
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

    void optimizePartly(int part_index, int parts_num)
    {
        for (int i = unit_next.size() * part_index / parts_num; i < unit_next.size() * (part_index + 1) / parts_num; i++) {
            unit_next.at(i).calcTotalDistance(points);
            unit_next.at(i).optimize(points, num_of_city);
            unit_next.at(i).calcFitness(num_of_city);
        };
    }

    double optimizePartlyFinaly(int part_index, int parts_num)
    {
        double min_distance = 100000000;
        for (int i = unit_now.size() * part_index / parts_num; i < unit_now.size() * (part_index + 1) / parts_num; i++) {
            auto distance_now = unit_now.at(i).optimizeFinaly(points, num_of_city);
            std::cout << min_distance << std::endl;
            min_distance = std::min(min_distance, distance_now);
        };
        return min_distance;
    }

    void client(int start, int end, bool is_final)
    {

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cout << "Error socket:" << std::strerror(errno);
            client(start, end, is_final);
            return;
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234);
        addr.sin_addr.s_addr = inet_addr("192.168.1.5");

        connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));

        if (start == -1) {
            int num = -1;
            send(sockfd, &num, sizeof(int), 0);  // serverに閉じてOKと送信
            return;
        }

        int num = end - start;
        send(sockfd, &num, sizeof(int), 0);

        send(sockfd, &is_final, sizeof(bool), 0);

        //optimizeしてほしいデータを送信
        auto sendUnit = [&](std::vector<Chromosome<NUM_OF_CITY>>& unit) {
            std::cerr << "start send" << std::endl;
            for (int i = start; i < end; ++i) {
                for (int div = 0; div < DIV; ++div) {
                    bool is = false;
                    int count = 0;
                    auto st = sizeof(Chromosome<NUM_OF_CITY>) * div / DIV;
                    auto ed = sizeof(Chromosome<NUM_OF_CITY>) * (div + 1) / DIV;
                    while (!is) {
                        ++count;
                        if (count > 10) {
                            div = DIV;
                            --num;
                            break;
                        }
                        assert(unit.at(i).total_distance > 1000);
                        send(sockfd, (void*)((char*)&unit.at(i) + st), ed - st, 0);
                        recv(sockfd, &is, sizeof(bool), 0);  //受信したら次を送ってOK
                    }
                }
            }
            std::cerr << "end send" << std::endl;
        };


        if (!is_final) {
            sendUnit(unit_next);
            //optimize後のデータを受信
            std::cerr << "start receive" << std::endl;
            for (int i = start; i < start + num; ++i) {
                for (int div = 0; div < DIV; ++div) {
                    bool is = false;
                    int count = 0;
                    auto st = sizeof(Chromosome<NUM_OF_CITY>) * div / DIV;
                    auto ed = sizeof(Chromosome<NUM_OF_CITY>) * (div + 1) / DIV;
                    while (!is) {
                        ++count;
                        if (count > 10) {
                            div = 0;
                            --num;
                            std::cout << "skip " << i << std::endl;
                            break;
                        }
                        is = recv(sockfd, (void*)((char*)&unit_next.at(i) + st), ed - st, 0) == ed - st;
                        send(sockfd, &is, sizeof(bool), 0);
                    }
                }
            }
            std::cerr << "end receive" << std::endl;
            for (int i = start + num; i < end; ++i) {
                unit_next.at(i) = best_chromosome;
            }
        } else {
            sendUnit(unit_now);
            bool is = false;
            while (!is) {
                is = recv(sockfd, &min_distances[6], sizeof(double), 0) == sizeof(double);
                send(sockfd, &is, sizeof(bool), 0);
            }
            std::cerr << "end receive" << std::endl;
        }

        close(sockfd);
    }

    void optimizePartlyByServer(int part_index_s, int part_index_e, int parts_num)
    {
        client(unit_next.size() * part_index_s / parts_num, unit_next.size() * (part_index_e + 1) / parts_num, false);
    }

    void optimizePartlyByServerFinaly(int part_index_s, int part_index_e, int parts_num)
    {
        client(unit_now.size() * part_index_s / parts_num, unit_now.size() * (part_index_e + 1) / parts_num, true);
    }

    void optimizeUnitNext()
    {
        //auto optimizePartly0 = [&]() { optimizePartlyByServer(0, 0, 6); };
        auto optimizePartly1 = [&]() { optimizePartly(8, 14); };
        auto optimizePartly2 = [&]() { optimizePartly(9, 14); };
        auto optimizePartly3 = [&]() { optimizePartly(10, 14); };
        auto optimizePartly4 = [&]() { optimizePartly(11, 14); };
        auto optimizePartly5 = [&]() { optimizePartly(12, 14); };
        auto optimizePartly6 = [&]() { optimizePartly(13, 14); };

        std::thread th1(optimizePartly1);
        std::thread th2(optimizePartly2);
        std::thread th3(optimizePartly3);
        std::thread th4(optimizePartly4);
        std::thread th5(optimizePartly5);
        std::thread th6(optimizePartly6);

        optimizePartlyByServer(0, 7, 14);
        //optimizePartly(0, 7);

        th1.join();
        th2.join();
        th3.join();
        th4.join();
        th5.join();
        th6.join();
    }

    void selection()
    {
        optimizeUnitNext();
        unit_next.push_back(best_chromosome);
        auto comp = [](const auto& a, const auto& b) {
            return a.total_distance < b.total_distance;
        };
        std::priority_queue<Chromosome<NUM_OF_CITY>, std::vector<Chromosome<NUM_OF_CITY>>, decltype(comp)> bestNp1;
        std::set<double> used;
        assert(unit_next.size() >= param.N_p1);

        for (auto& chromo : unit_next) {
            double distance = chromo.total_distance;
            if (used.count(distance)) {
                continue;
            } else {
                used.insert(distance);
                used.insert(distance - 0.1);
                used.insert(distance - 0.2);
                used.insert(distance + 0.1);
                used.insert(distance + 0.2);
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
        fitness_ave = 0;

        while (!bestNp1.empty()) {
            unit_now.push_back(bestNp1.top());
            fitness_ave += bestNp1.top().fitness;
            bestNp1.pop();
        }

        best_chromosome = unit_now.back();
        fitness_ave = fitness_ave / param.N_p1;
    }

    void edgeExchange(int parent1, int parent2)
    {
        assert(parent1 < unit_next.size());
        assert(parent2 < unit_next.size());
        int locus1_s = distr(eng) % num_of_city;
        int locus2_s = unit_next.at(parent2).getLocusByFirstCodon(
            unit_next.at(parent1).chromosome.at(locus1_s).codon1, num_of_city);
        assert(unit_next.at(parent1).chromosome.at(locus1_s).codon1 == unit_next.at(parent2).chromosome.at(locus2_s).codon1);
        while (true) {
            int locus1_g = unit_next.at(parent1).edgeReverse(
                locus1_s, unit_next.at(parent2).chromosome.at(locus2_s).codon2,
                num_of_city);
            int locus2_g = unit_next.at(parent2).edgeReverse(
                locus2_s, unit_next.at(parent1).chromosome.at(locus1_s).codon2,
                num_of_city);
            std::swap(unit_next.at(parent1).chromosome.at(locus1_s),
                unit_next.at(parent2).chromosome.at(locus2_s));
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
            int num_of_child = param.N_p2 / param.N_p1 * chromo.fitness / fitness_ave;
            for (int i = 0; i < num_of_child; i++) {
                unit_next.push_back(chromo);
            }
        }
    }

    void crossOverNextUnit()
    {
        // edgeeExchange and selection
        // mutation
        std::shuffle(unit_next.begin(), unit_next.end(), eng);
        int mutation_num = (int)unit_next.size() / param.mutation_rate;
        for (int i = mutation_num; 2 * i + 1 < unit_next.size(); ++i) {
            edgeExchange(2 * i, 2 * i + 1);
        }

        for (int i = 0; i < mutation_num; ++i) {
            for (int j = 0; j < 5; ++j) {
                int rand1 = distr(eng) % num_of_city;
                int rand2 = distr(eng) % num_of_city;
                unit_next.at(i).mutation(rand1, rand2, num_of_city);  // random
            }
        }
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

        auto optimizeFinaly0 = [&]() { min_distances.at(0) = optimizePartlyFinaly(0, 15); };
        auto optimizeFinaly1 = [&]() { min_distances.at(1) = optimizePartlyFinaly(1, 15); };
        auto optimizeFinaly2 = [&]() { min_distances.at(2) = optimizePartlyFinaly(2, 15); };
        auto optimizeFinaly3 = [&]() { min_distances.at(3) = optimizePartlyFinaly(3, 15); };
        auto optimizeFinaly4 = [&]() { min_distances.at(4) = optimizePartlyFinaly(4, 15); };
        auto optimizeFinaly5 = [&]() { min_distances.at(5) = optimizePartlyFinaly(5, 15); };

        optimizePartlyByServerFinaly(6, 14, 15);

        std::thread th0(optimizeFinaly0);
        std::thread th1(optimizeFinaly1);
        std::thread th2(optimizeFinaly2);
        std::thread th3(optimizeFinaly3);
        std::thread th4(optimizeFinaly4);
        std::thread th5(optimizeFinaly5);


        th0.join();
        th1.join();
        th2.join();
        th3.join();
        th4.join();
        th5.join();

        client(-1, -1, false);

        auto min_distance = min_distances.at(0);
        for (auto d : min_distances) {
            min_distance = std::min(min_distance, d);
        }

        return min_distance;
    }


    std::vector<Chromosome<NUM_OF_CITY>> unit_next;
    std::vector<Chromosome<NUM_OF_CITY>> unit_now;
    Chromosome<NUM_OF_CITY> best_chromosome;
    double fitness_ave;

    std::vector<Point> points;
    int num_of_city;

    std::array<double, 7> min_distances;
};


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
