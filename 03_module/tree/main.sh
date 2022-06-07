#!/bin/bash
g++ main.cpp read_line.cpp read_token.cpp
if [ $? != 0 ]; then
	echo "failed to compile." >&2
	exit 1
fi
if [ -e result.txt ]; then
	rm result.txt
fi
read -p "Press is_testcase : " is_testcase
if [ $is_testcase == 1 ]; then
    ./a.out < testcase.txt > result.txt
else
    ./a.out > result.txt
fi
