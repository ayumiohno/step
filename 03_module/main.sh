#!/bin/bash
g++ main.cpp read_line.cpp read_token.cpp
if [ $? != 0 ]; then
	echo "failed to compile." >&2
	exit 1
fi
if [ -e result.txt ]; then
	rm result.txt
fi
./a.out > result.txt
