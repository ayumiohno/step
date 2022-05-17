#!/bin/bash
g++ main.cpp data.hpp node.hpp list.hpp cache.hpp list.cpp data_base.hpp -o a.out
if [ $? != 0 ]; then
	echo "failed to compile." >&2
	exit 1
fi
if [ -e result.txt ]; then
	rm result.txt
fi
./a.out > result.txt
