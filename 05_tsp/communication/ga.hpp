#pragma once
#include "chromosome.hpp"
#include "macro_gene.hpp"
#include "point.hpp"
#include "tsp.hpp"
#include <algorithm>
#include <arpa/inet.h>  //バイトオーダの変換に利用
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
std::default_random_engine eng(rd());
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

    void client(int start, int end)
    {

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            std::cout << "Error socket:" << std::strerror(errno);
            client(start, end);
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

        //optimizeしてほしいデータを送信
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
                        std::cout << "skip " << i << std::endl;
                        --num;
                        break;
                    }
                    assert(unit_next.at(i).total_distance > 1000);
                    auto size = send(sockfd, (void*)((char*)&unit_next.at(i) + st), ed - st, 0);
                    std::cout << size << " " << ed - st << std::endl;
                    recv(sockfd, &is, sizeof(bool), 0);  //受信したら次を送ってOK
                }
            }
        }

        //optimize後のデータを受信
        for (int i = start; i < start + num; ++i) {
            std::cerr << i << std::endl;
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
        for (int i = start + num; i < end; ++i) {
            unit_next.at(i) = best_chromosome;
        }
        close(sockfd);
    }

    void optimizePartlyByServer(int part_index_s, int part_index_e, int parts_num)
    {
        client(unit_next.size() * part_index_s / parts_num, unit_next.size() * (part_index_e + 1) / parts_num);
    }

    void optimizeUnitNext()
    {
        //auto optimizePartly0 = [&]() { optimizePartlyByServer(0, 0, 6); };
        auto optimizePartly1 = [&]() { optimizePartly(6, 12); };
        auto optimizePartly2 = [&]() { optimizePartly(7, 12); };
        auto optimizePartly3 = [&]() { optimizePartly(8, 12); };
        auto optimizePartly4 = [&]() { optimizePartly(9, 12); };
        auto optimizePartly5 = [&]() { optimizePartly(10, 12); };
        auto optimizePartly6 = [&]() { optimizePartly(11, 12); };

        std::thread th1(optimizePartly1);
        std::thread th2(optimizePartly2);
        std::thread th3(optimizePartly3);
        std::thread th4(optimizePartly4);
        std::thread th5(optimizePartly5);
        std::thread th6(optimizePartly6);

        optimizePartlyByServer(0, 5, 12);
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
        client(-1, -1);
        std::vector<Point> best_points;
        double best_distance = best_chromosome.total_distance;
        std::cout << "best_distance: " << best_distance << std::endl;
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


    std::vector<Chromosome<NUM_OF_CITY>> unit_next;
    std::vector<Chromosome<NUM_OF_CITY>> unit_now;
    Chromosome<NUM_OF_CITY> best_chromosome;
    double fitness_ave;

    std::vector<Point> points;
    int num_of_city;
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
