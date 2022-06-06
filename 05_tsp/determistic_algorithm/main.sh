#!/bin/bash
read -p "Press file number : " file_num
if [ -e output_$file_num.csv ]; then
	rm output_$file_num.csv
fi
if [ ! -e a.out ] || [ a.out -ot main.cpp ]; then
	g++ -O2 main.cpp tsp.cpp -o a.out
	if [ $? != 0 ]; then
		echo "failed to compile." >&2
		exit 1
	fi
fi
./a.out <<EOS > output_$file_num.csv
$file_num
EOS
