Page Rank
=

# 目的
* testcase/pages(_small).txt, links(_small).txtのデータをもとに、各page(node)のpage rankを出力
* pages.txt : idとpage名を対応させる
* links.txt : idとそれが参照しているページのidを対応させる
* page rank : 数パーセントはrandomに(全pageに均等)割り振られ、各nodeが保持している残りのpointは参照しているページに均等に割り振られ、全割り振りが終了後に合計される


# 実装
## 概要
1. fileの読み込み
2. page rankの更新 \
2.1. link先へのpointの割り振り\
2.2. link元からのpointの合計\
3. rankの出力

## 1.fileの読み込み
* pages.txt, links.txtに共通する読み込み処理は共通のreadFile関数で行われる。
* データへの追加時の個別処理は、readFileに渡すstd::function<void(std::string, std::string)>で行われる。
* pages.txtのデータは、id(uint32_t)をkey, page名(std::string)をvalueとしたunordered_mapで管理
* links.txtのデータは、id(uint32_t)をkey, 自身のpointと参照しているページのidのstd::pair<uint32_t, uint32_t>をvalueとしたunordered_mapで管理。
* 参照しているページはidをkey(uint32_t), そのedgeに割り振られたpoint(uint16_t)をvalueとしたunordered_setで管理

### 1-sub multi threadの場合
* links.txtを3分割
* linksへのデータ追加だけ、mutexを使用

## 2.1 link先へのpointの割り振り
* distributionPointsToEdgesで実装されている

## 2.2 link元からのpointの合計
* sumPointsFromEdgesで実装されている

## 3. rankの出力
* printAllRank : 各page名とpointをpageのidのhash順に出力
* printTopXPages : top x個のpage id とpointをpriority_queueで管理し、最後にpage名とpointを出力

# 使い方
* pages.txt, links.txtを../testcase/に置く
* 動作確認済み環境
    * wsl2
    * ubuntu20.04LTS
    * g++ 9.4.0
    * zsh
## 通常
* shell scriptを実行できる場合(zsh or bash?):
```
./main.sh
```
* 実行できない場合
```
g++ -std=c++2a main.cpp page_rank.cpp
./a.out
```
## マルチスレッド
* links.txtの分割
    * linuxコマンドを用いて3分割する場合、以下のコマンドを実行後、file名の末尾につく数字を'div'と'.'の間に移動
```
split -l 10594735 -d --verbose links_divide.txt links_div.txt
```

* shell scriptを実行できる場合(zsh or bash?):
```
./main_multi_thread.sh
```
* 実行できない場合
```
g++ -std=c++2a main.cpp page_rank.cpp　-pthread
./a.out
```
## 注意
* c++ 20のstdが必要
    * priority queueをpairに対して使う際にlambda式をテンプレート引数に入れてるため(?)
* multi threadの場合 -pthreadオプションが必要
# 結果
* rankがtop10のページ名を小さい順にresult_top10.txtに
* すべてのページ名とrankをidのhash順にresult.txtに出力した。
* fileの読み込みが大幅な律速(1~3分)
* pointのsummationも1loopにつき45sほどかかる
* マルチスレッドが逆効果！+1～２分(mutexのせい？)
