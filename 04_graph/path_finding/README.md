DFS BFS 
=

# 目的
* testcase/pages(_small).txt, links(_small).txtのデータをもとに、startとgoalを結ぶpathが存在するか、存在するならそのpathが何かを出力する。
* pages.txt : idとpage名を対応させる
* links.txt : idとそれが参照しているページのidを対応させる
* start, goal : page名を対応させる

# 実装
## 概要
1. fileの読み込み
2. start, goalのpage名からidを求める。
3. 探索
4. pathの出力

※ 3の探索はdfsとbfsの二種類で行われる。2, 4 はそれぞれの探索をする関数に付随される。

## 1.fileの読み込み
* pages.txt, links.txtに共通する読み込み処理は共通のreadFile関数で行われる。
* データへの追加時の個別処理は、readFileに渡すstd::function<void(std::string, std::string)>で行われる。
* pages.txtのデータは、id(uint32_t)をkey, page名(std::string)をvalueとしたunordered_mapで管理
* links.txtのデータは、id(uint32_t)をkey, 参照しているページのidをvalueとしたunordered_mapで管理。
* 参照しているページのidはuint32_tをkeyとしたunordered_setで管理

## 2. start, goalのpage名からidを求める
* path_finding.cppのfindKey()で実装されている

## 3. 探索
* dfsではstack, bfsではqueueを用いて、探索するnodeを管理
* dfsでは探索済みのnodeのidをunordered_setで管理
* bfsでは探索済みのnode、およびその前に訪問したnodeのidをunordered_mapで管理
    * bfsではdfsと異なり、pathを別途管理する必要があるため、pathの管理と訪問済みnodeの管理を一体化した

## 4. pathの出力
* idからpage名を求めて出力

# 使い方
* pages.txt, links.txtを../testcase/に置く
* shell scriptを実行できる場合(zsh or bash?):
```
./main.sh
```
* 実行できない場合
```
g++ main.cpp path_finding.cpp
./a.out
```
* 動作確認済み環境
    * wsl2
    * ubuntu20.04LTS
    * g++ 9.4.0
    * zsh

# 結果
* _small.txtの結果をresult_small.txtに
* .txtの結果をresult.txtに出力した。
* fileの読み込みが大幅な律速(1分強)