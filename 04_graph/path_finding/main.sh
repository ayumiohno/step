#!/bin/bash
if [ -e result.txt ]; then
	rm result.txt
fi
g++ -O2 main.cpp path_finding.cpp -o a.out
if [ $? != 0 ]; then
	echo "failed to compile." >&2
	exit 1
fi
./a.out > result.txt
