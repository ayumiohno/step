TSP
=
# 概要
1. Genetic Algorithm による大域的な探索
2. 2-opt, 3-opt などによる局所的最適化
3. multi threadと分散処理(ソケット通信を使用)による高速化

# Genetic Algorithmについて
* 下記の文献の理論に準拠して実装した
>前川景示, 玉置久, 喜多一, & 西川〓 一. (1995). 遺伝アルゴリズムによる巡回セールスマン問題の一解法. 計測自動制御学会論文集, 31(5), 598-605.

* 

# 局所最適化について

# ソケット通信を用いた分散処理について
* 通信部分の実装は以下のサイトを参考にした
> https://tora-k.com/2019/08/27/socket-c/
* 以下のプロトコル(のようなもの)に基づいて分散処理を行った

# 実行方法
1. 使用するinput file番号と、chromosomeクラスをsocket通信で送る際の分割数をmain.cpp, server_for_ga.cpp, ga.hppの冒頭に代入
2. server pcのipアドレスを代入
3. 実行
* main pc
```
g++ -std=c++2a -O3 main.cpp tsp.cpp -pthread -o main.out
./main.out
```
* sub pc
```
g++ -std=c++2a -O3 server_for_ga.cpp tsp.cpp -pthread -o server.out
./server.out
```
# 結果
* best_score : 40862.4
* seed : -1967621186 (default_random_engineに渡したもの)