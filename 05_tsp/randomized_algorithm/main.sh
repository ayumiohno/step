#!/bin/bash
if [ -e file.out ]; then
	rm file.out
fi
g++ -O2 main.cpp tsp.cpp -o a.out
if [ $? != 0 ]; then
	echo "failed to compile." >&2
	exit 1
fi
for i in `seq 1 10`
do
./a.out $i
done >>file.out
cat file.out | awk 'BEGIN{m=1000000} {if($1 != "" && m>$1) m=$1} END{print m}'
