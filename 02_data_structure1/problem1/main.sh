#!/bin/sh
if [ -e file.out ]; then
	rm file.out
fi

for i in `seq 1 100`
do
python3 main.py $i
done >>file.out

gnuplot << EOF
set terminal png
set output "Plot.png"
plot "file.out" using 1:2
exit
EOF
