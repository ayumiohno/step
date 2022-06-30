#include "chromosome.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

constexpr int FILE_NUM = 4;
constexpr std::array<int, 8> NUMS = {5, 8, 16, 64, 128, 512, 2048, 8192};
constexpr int NUM_OF_CITY = NUMS.at(FILE_NUM);

int main()
{
    std::vector<Point> points;
    {
        auto file_name = "../testcase/input_" + std::to_string(FILE_NUM) + ".csv";
        std::ifstream file(file_name);
        std::string data;
        while (std::getline(file, data)) {
            auto index = data.find(',');
            auto x = data.substr(0, index);
            auto y = data.substr(index + 1, data.size() - x.size() - 1);
            try {
                points.push_back(
                    Point{(double)(std::stold(x)), (double)(std::stold(y))});
            } catch (const std::invalid_argument& e) {
            }
        }
        file.close();
    }

    assert(points.size() == NUM_OF_CITY);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {

        std::cout << "Error socket:" << std::strerror(errno);
        return -1;
    }

    //アドレスの生成
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = inet_addr("192.168.1.5");

    //ソケット登録
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {

        std::cout << "Error bind:" << std::strerror(errno);
        return -1;
    }

    while (true) {


        if (listen(sockfd, SOMAXCONN) < 0) {

            std::cout << "Error listen:" << std::strerror(errno);
            close(sockfd);
            usleep(10000);
            continue;
        }

        //接続待ち
        struct sockaddr_in get_addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int connect = accept(sockfd, (struct sockaddr*)&get_addr, &len);


        if (connect < 0) {                                         //エラー処理
            std::cout << "Error accept:" << std::strerror(errno);  //標準出力
            usleep(10000);
            continue;  //異常終了
        }

        int num;
        recv(connect, &num, sizeof(int), 0);  //送られてくるデータ数
        std::cout << " NUM: " << num << std::endl;

        if (num == -1) {  // num = -1　の時serverを閉じる
            break;
        }

        //受信
        std::vector<Chromosome<NUM_OF_CITY>*> chromos;
        for (int i = 0; i < num; ++i) {
            auto chromo = new Chromosome<NUM_OF_CITY>{};
            chromos.push_back(chromo);
            bool is = false;
            while (!is) {
                is = recv(connect, chromos[i], sizeof(Chromosome<NUM_OF_CITY>), 0) == sizeof(Chromosome<NUM_OF_CITY>);
                send(connect, &is, sizeof(bool), 0);
            }
        }


        //optimize
        int th_num = 7;
        auto optimizePartly = [&](int index) {
            int start = index * num / th_num;
            int end = (index + 1) * num / th_num;
            for (int i = start; i < end; ++i) {
                chromos[i]->calcTotalDistance(points);
                chromos[i]->optimize(points, points.size());
                chromos[i]->calcFitness(points.size());
            }
        };
        auto optimizePartly1 = [&]() { optimizePartly(1); };
        auto optimizePartly2 = [&]() { optimizePartly(2); };
        auto optimizePartly3 = [&]() { optimizePartly(3); };
        auto optimizePartly4 = [&]() { optimizePartly(4); };
        auto optimizePartly5 = [&]() { optimizePartly(5); };
        auto optimizePartly6 = [&]() { optimizePartly(6); };
        std::thread th1(optimizePartly1);
        std::thread th2(optimizePartly2);
        std::thread th3(optimizePartly3);
        std::thread th4(optimizePartly4);
        std::thread th5(optimizePartly5);
        std::thread th6(optimizePartly6);

        optimizePartly(0);

        th1.join();
        th2.join();
        th3.join();
        th4.join();
        th5.join();
        th6.join();

        for (int i = 0; i < num; ++i) {
            bool is = false;
            while (!is) {
                send(connect, chromos[i], sizeof(Chromosome<NUM_OF_CITY>), 0);
                recv(connect, &is, sizeof(bool), 0);
            }
        }
        ///optimize後のデータを送信
        for (int i = 0; i < num; ++i) {
            delete chromos[i];
        }

        close(connect);
    }

    close(sockfd);

    return 0;
}
