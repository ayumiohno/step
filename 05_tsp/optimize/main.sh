#!/bin/bash
read -p "Press file number : " file_num
if [ -e output_$file_num.csv ]; then
	rm output_$file_num.csv
fi
g++ -O2 main.cpp tsp.cpp -o a.out
if [ $? != 0 ]; then
	echo "failed to compile." >&2
	exit 1
fi
for i in `seq 1 10`
do
./a.out <<EOS
$file_num
EOS
done >>output_$file_num.csv
cat output_$file_num.csv | awk 'BEGIN{m=1000000} {if($1 != "" && m>$1) m=$1} END{print m}'
