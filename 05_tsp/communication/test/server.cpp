#include "framework.hpp"
#include <arpa/inet.h>  //バイトオーダの変換に利用
#include <cstring>
#include <iostream>      //標準入出力
#include <string>        //string型
#include <sys/socket.h>  //アドレスドメイン
#include <sys/types.h>   //ソケットタイプ
#include <unistd.h>      //close()に利用

int main()
{

    //ソケットの生成
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  //アドレスドメイン, ソケットタイプ, プロトコル
    if (sockfd < 0) {                              //エラー処理

        std::cout << "Error socket:" << std::strerror(errno);  //標準出力
        exit(1);                                               //異常終了
    }

    //アドレスの生成
    struct sockaddr_in addr;                          //接続先の情報用の構造体(ipv4)
    memset(&addr, 0, sizeof(struct sockaddr_in));     //memsetで初期化
    addr.sin_family = AF_INET;                        //アドレスファミリ(ipv4)
    addr.sin_port = htons(1234);                      //ポート番号,htons()関数は16bitホストバイトオーダーをネットワークバイトオーダーに変換
    addr.sin_addr.s_addr = inet_addr("192.168.1.5");  //IPアドレス,inet_addr()関数はアドレスの翻訳

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
        int connect = accept(sockfd, (struct sockaddr*)&get_addr, &len);  //接続待ちソケット, 接続相手のソケットアドレスポインタ, 接続相手のアドレスサイズ

        if (connect < 0) {  //エラー処理

            std::cout << "Error accept:" << std::strerror(errno);  //標準出力
            exit(1);                                               //異常終了
        }

        int num;
        recv(connect, &num, sizeof(int), 0);  //受信
        std::cout << " NUM: " << num << std::endl;
        //send(connect, num, sizeof(int), 0);
        if (num == -1) {
            break;
        }

        //受信
        std::vector<MacroGene*> genes;
        for (int i = 0; i < 10; ++i) {
            auto gene = new MacroGene{};
            genes.push_back(gene);
            recv(connect, genes[i], 16, 0);                                         //受信
            std::cout << genes[i]->codon1 << " " << genes[i]->codon2 << std::endl;  //標準出力
        }

        for (int i = 0; i < 10; ++i) {
            genes[i]->inverse();
        }

        //送信
        for (int i = 0; i < 10; ++i) {
            send(connect, genes[i], 16, 0);  //送信
        }
        //std::cout << str << std::endl;  //標準出力
        close(connect);
    }
    //ソケットクローズ
    close(sockfd);

    return 0;
}
