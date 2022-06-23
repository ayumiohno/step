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
