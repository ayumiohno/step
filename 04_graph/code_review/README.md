# Class 4 宿題

## 課題
Google から 渋谷 をたどる経路をDFSとBFSで探す

---
## 答えと実行結果

答え：経路は存在して、２ステップでたどり着ける  
BFSで計算した最短経路の一つは、  
Google -> スターバックス -> 渋谷　　  
(DFSではかなり長い経路が出力されました。)

---
## 実行するファイル名

BFS : wikipedia_sample_bfs.cc  
コードのリンク：https://gist.github.com/nakagaito/1eb6f037de795a8496757520017f1151
  
DFS : wikipedia_sample_dfs.cc  
コードのリンク：https://gist.github.com/nakagaito/64b044ed11aeda37d37dcf2836d747c3

---
## ディレクトリ構成
[https://github.com/yukidmy/step_wikipedia-graph](https://github.com/yukidmy/step_wikipedia-graph)  
に書いてあるのと同じディレクトリ構成でpages.txt, links.txtを配置します。
  
┣━ data  
┃ &emsp;&emsp;┣━ links.txt  
┃ &emsp;&emsp;┗━ pages.txt  
┣━ wikipedia_sample_bfs.cc  
┗━ wikipedia_sample_dfs.cc  

---
## 実行方法

1. 上に書いてあるディレクトリ構成でpages.txt, links.txtを配置します。

2. BFSの実行：
```
g++ wikipedia_sample_bfs.cc  
./a.out 
```
  
3. DFSの実行：
```
g++ wikipedia_sample_dfs.cc  
./a.out 
```

(Windowsで実行確認しました。)

## 実装

c++ で実装しました。  
デバッグ出力用に、debug(bool)変数を用意し、これがtrueならばデバッグ出力を行い、falseならば出力しないようにしました。  

1. ファイルの読み込み：
  ほぼサンプルファイル(wikipedia_sample.cc)通りです。少し型の変換で変更しました。

2. 探索    
BFSの場合: Queueを使って、探索ノードをfront()で取り出して調べます。  経路を保存するために、ひとつ前に訪れたノードID(int)をおぼえておくベクター配列prev_node<int>を用意し、これに保存します。  
DFSの場合： Stackを使って、探索ノードをtop()で取り出して調べます。  
BFSとDFSの違いは、Queue/Stackと取り出しメソッドがfront()/top()の違いのみです。

3. 経路の出力:
  prev_nodeに保存してある、ひとつ前のノードIDを辿って、経路を出力します。 
  
