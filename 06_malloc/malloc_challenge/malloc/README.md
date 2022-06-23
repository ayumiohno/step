# Real malloc challenge!

# my desciption
## 概要
* **best_score.cのみを見てください**
* best_fit_malloc.c
    * 全探索でbest_fitを見つけるもの
* freelist.c
    * sizeを基準に4分割してbest_fitを見つけるもの
* binary_tree.c
    * sizeを基準にbinary treeでbest_fitをみつけるもの
* merge.c
    * binary_treeの各nodeにbefore, afterを加え、O(1)でmergeするもの
* merge_and_freelist.c
    * 小さいsizeに対してはfree_listを適応するもの
    * freelistに対してはmergeなし
* best_score.c
    * 最もscoreがよかったもの
    * 唯一整えたもの
    * merge_and_freelist.cに以下の機能を追加
        * freelistにもmergeを追加(全探索)
        * cache(average - 15 ~ averageを同一のmetadataで管理)を追加
* develop.c
    * 開発中
    * やろうとしていること：空き領域のmergeの改良        

## best_score.cの機能の説明
### binary tree
* best_fitはsizeを参考にbinary treeを下っていくことで、平均計算量O(log n)で見つけられる
* mergeする相手は、各nodeのbefore, afterを用いれば、計算量O(1)で見つけられる。
* sample codeを一部変更し、remainig sizeがsizeof(my_metadata_t)より小さい時、metadata->sizeをsizeに変更していない。
    * これにより、metadataに管理されていない領域がなくなる
* 挿入は平均O(log n)
### free list
* sample codeと同様のlinked list
* best_fitは全探索O(n)
* mergeする相手も全探索O(n) 
* 挿入はO(1)
* binary_treeよりmetadataのsizeが小さいので、小さい
sizeの要求向け
### cache
* 初めの50回に取った平均~ 平均-15に対して、一律に平均のsizeのmetadataを渡す。
* sizeが同一なため、各nodeがsizeを管理する必要がなく、nextポインタのみを持てばいいので、structのsizeが小さくなる
* 4096 biteを確保して、それを分けていく段階で生じるcacheとsizeが異なる領域はcache_poolで管理
* 使う領域の選び方
    * cacheの在庫があるときはそれを用いる
    * poolが余っている場合は、その末端からcacheを生成
    * poolが余っていない時は、新たにpoolを生成
    * いずれにせよO(1)
* 挿入もO(1)

### 使い分け
* cacheの対象サイズのとき
    * cache あるいはcache_poolの在庫がある時:cache
    * smallあるいはlargeで新たに領域を確保せずに済む場合:binary tree or free list
* sizeが128より大きい場合
    * binary treee
* sizeが128以下の場合        
    * binary treeに128以下の空き領域がある場合:malloc_large
    * それ以外:free_list
## best_score.cの結果

```
====================================================
Challenge #1    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|               8 =>               7
Utilization [%] |              70 =>              73
====================================================
Challenge #2    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|               5 =>               6
Utilization [%] |              40 =>              48
====================================================
Challenge #3    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|             134 =>             225
Utilization [%] |               7 =>              49
====================================================
Challenge #4    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|           30960 =>             131
Utilization [%] |              16 =>              74
====================================================
Challenge #5    |   simple_malloc =>       my_malloc
--------------- + --------------- => ---------------
       Time [ms]|           21976 =>              92
Utilization [%] |              15 =>              73

Challenge done!
Please copy & paste the following data in the score sheet!
7,73,6,48,225,49,131,74,92,73,
```

# default desciption
## Instruction

Your task is implement a better malloc logic in [malloc.c](malloc.c) to improve the speed and memory usage.

## How to build & run a benchmark

```
# build
make
# run a benchmark
make run
```

If the commands above don't work, you can build and run the challenge directly by running:

```
gcc -Wall -O3 -lm -o malloc_challenge.bin main.c malloc.c simple_malloc.c
./malloc_challenge.bin
```

## Acknowledgement

This work is based on [xharaken's malloc_challenge.c](https://github.com/xharaken/step2/blob/master/malloc_challenge.c). Thank you haraken-san!
