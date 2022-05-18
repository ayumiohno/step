行列の計算時間
=

# 目的
1. 2つのn×n行列の積の計算時間tを求める
2. nとtをplotする
# 概要
## main.py
* 目的1を行う
* 行列の積は、$c_{i,j}=\sum_{k=0}^n=a_{i,k}b_{k,j}$を用いて、要素ごとに計算した
* $n$と計算時間$t$を出力した
    * plotの仕様の関係でsample codeから変更した
## main.sh
* 目的2を行う
1. $n=1:100$でmain.pyを実行
2. 実行結果を file.out に保存
3. gnuplotでplotし、plot.png に保存
# 工夫した点
* shell scriptとgnuplotを用いて、pythonプログラムの実行とグラフの作成を自動化したこと
# 結果
* グラフは plot.png にある
* グラフよりO(n^2)であることが確かめられた