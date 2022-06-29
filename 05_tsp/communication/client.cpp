#include "framework.hpp"
#include <arpa/inet.h>  //バイトオーダの変換に利用
#include <cstring>
#include <iostream>      //標準入出力
#include <string>        //string型
#include <sys/socket.h>  //アドレスドメイン
#include <sys/types.h>   //ソケットタイプ
#include <unistd.h>      //close()に利用

int client(bool is_finish)
{

    //ソケットの生成
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  //アドレスドメイン, ソケットタイプ, プロトコル
    if (sockfd < 0) {                              //エラー処理

        std::cout << "Error socket:" << std::strerror(errno);  //標準出力
        exit(1);                                               //異常終了
    }

    //アドレスの生成
    struct sockaddr_in addr;                        //接続先の情報用の構造体(ipv4)
    memset(&addr, 0, sizeof(struct sockaddr_in));   //memsetで初期化
    addr.sin_family = AF_INET;                      //アドレスファミリ(ipv4)
    addr.sin_port = htons(1234);                    //ポート番号,htons()関数は16bitホストバイトオーダーをネットワークバイトオーダーに変換
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //IPアドレス,inet_addr()関数はアドレスの翻訳

    //ソケット接続要求
    connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));  //ソケット, アドレスポインタ, アドレスサイズ

    if (is_finish) {
        int num = -1;
        send(sockfd, &num, sizeof(int), 0);  //送信
        return 0;
    }

    int num = 10;
    send(sockfd, &num, sizeof(int), 0);  //送信
    //recv(sockfd, &num, sizeof(int), 0);

    //データ送信
    std::vector<MacroGene> genes(10);
    for (int i = 0; i < 10; ++i) {
        //auto gene = new MacroGene{};
        genes[i].codon1 = 1;
        genes[i].codon2 = 2;
        //genes.push_back(gene);
        send(sockfd, &genes[i], 16, 0);                                       //送信
        std::cout << genes[i].codon1 << " " << genes[i].codon2 << std::endl;  //標準出力
    }

    //データ受信
    //std::vector<MacroGene*> r_genes;
    for (int i = 0; i < 10; ++i) {
        //auto gene = new MacroGene{};
        //r_genes.push_back(gene);
        recv(sockfd, &genes[i], 16, 0);                                       //受信
        std::cout << genes[i].codon1 << " " << genes[i].codon2 << std::endl;  //標準出力
    }

    //ソケットクローズ
    close(sockfd);

    return 0;
}

int main()
{
    for (int i = 0; i < 3; ++i) {
        client(false);
    }
    client(true);
    return 0;
}
