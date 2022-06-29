#include "chromosome.hpp"
#include <arpa/inet.h>  //バイトオーダの変換に利用
#include <cstring>
#include <fstream>
#include <iostream>  //標準入出力
#include <sstream>
#include <stdio.h>
#include <string>        //string型
#include <sys/socket.h>  //アドレスドメイン
#include <sys/types.h>   //ソケットタイプ
#include <thread>
#include <unistd.h>  //close()に利用
#include <vector>

constexpr int NUM_OF_CITY = 128;

int main()
{
    std::vector<Point> points;
    {
        std::cerr << "put file number" << std::endl;
        std::string file_num;
        std::cin >> file_num;
        auto file_name = "../google-step-tsp/input_" + file_num + ".csv";
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

    //ソケットの生成
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  //アドレスドメイン, ソケットタイプ, プロトコル
    if (sockfd < 0) {                              //エラー処理

        std::cout << "Error socket:" << std::strerror(errno);  //標準出力
        exit(1);                                               //異常終了
    }

    //アドレスの生成
    struct sockaddr_in addr;                        //接続先の情報用の構造体(ipv4)
    memset(&addr, 0, sizeof(struct sockaddr_in));   // memsetで初期化
    addr.sin_family = AF_INET;                      //アドレスファミリ(ipv4)
    addr.sin_port = htons(1234);                    //ポート番号,htons()関数は16bitホストバイトオーダーをネットワークバイトオーダーに変換
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // IPアドレス,inet_addr()関数はアドレスの翻訳

    //ソケット登録
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {  //ソケット, アドレスポインタ, アドレスサイズ //エラー処理

        std::cout << "Error bind:" << std::strerror(errno);  //標準出力
        exit(1);                                             //異常終了
    }

    while (true) {
        //受信待ち
        if (listen(sockfd, SOMAXCONN) < 0) {  //ソケット, キューの最大長 //エラー処理

            std::cout << "Error listen:" << std::strerror(errno);  //標準出力
            close(sockfd);                                         //ソケットクローズ
            exit(1);                                               //異常終了
        }

        //接続待ち
        struct sockaddr_in get_addr;                                      //接続相手のソケットアドレス
        socklen_t len = sizeof(struct sockaddr_in);                       //接続相手のアドレスサイズ
        int connect = accept(sockfd, (struct sockaddr*)&get_addr, &len);  //接続待ちソケット, 接続相手のソケットアドレスポインタ,
                                                                          //接続相手のアドレスサイズ

        if (connect < 0) {  //エラー処理

            std::cout << "Error accept:" << std::strerror(errno);  //標準出力
            exit(1);                                               //異常終了
        }

        int num;
        recv(connect, &num, sizeof(int), 0);  //受信
        std::cout << " NUM: " << num << std::endl;
        // send(connect, num, sizeof(int), 0);
        if (num == -1) {
            break;
        }

        //受信
        std::vector<Chromosome<NUM_OF_CITY>*> chromos;
        for (int i = 0; i < num; ++i) {
            //uleep(100);
            auto chromo = new Chromosome<NUM_OF_CITY>{};
            chromos.push_back(chromo);
            recv(connect, chromos[i], sizeof(Chromosome<NUM_OF_CITY>), 0);  //受信
            bool is = true;
            send(connect, &is, sizeof(bool), 0);
            //std::cerr << "receive: " << i << " " << chromos[i]->total_distance << std::endl;
        }

        for (int i = 0; i < num; ++i) {
            chromos[i]->calcTotalDistance(points);
            chromos[i]->optimize(points, points.size());
            chromos[i]->calcFitness(points.size());
            //std::cerr << "opt: " << i << std::endl;
        }

        //送信
        int th_num = 1;
        auto optimizePartly = [&](int index) {
            int start = index * num / th_num;
            int end = (index + 1) * num / th_num;
            for (int i = start; i < end; ++i) {
                //usleep(10000);
                send(connect, chromos[i], sizeof(Chromosome<NUM_OF_CITY>), 0);  //送信
                bool is;
                recv(connect, &is, sizeof(bool), 0);
            }
        };
        auto optimizePartly0 = [&]() { optimizePartly(0); };
        std::thread th0(optimizePartly0);

        th0.join();

        for (int i = 0; i < num; ++i) {
            delete chromos[i];
            //std::cerr << "delete: " << i << std::endl;
        }

        // std::cout << str << std::endl;  //標準出力
        close(connect);
    }
    //ソケットクローズ
    close(sockfd);

    return 0;
}
