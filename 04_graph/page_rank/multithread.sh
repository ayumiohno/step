#!/bin/bash
g++ -std=c++2a main_multi_thread.cpp page_rank.cpp -pthread -o multithread.out
if [ $? != 0 ]; then
	echo "failed to compile." >&2
	exit 1
fi
./a.out
