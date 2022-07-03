gcc ijk.c -o ijk.out
gcc ikj.c -o ikj.out
gcc jik.c -o jik.out
gcc jki.c -o jki.out
gcc kij.c -o kij.out
gcc kji.c -o kji.out

if [ -e ijk.txt ]; then
	rm ijk.txt
fi
if [ -e ikj.txt ]; then
	rm ikj.txt
fi
if [ -e jik.txt ]; then
	rm jik.txt
fi
if [ -e jki.txt ]; then
	rm jki.txt
fi
if [ -e kij.txt ]; then
	rm kij.txt
fi
if [ -e kji.txt ]; then
	rm kji.txt
fi

for i in `seq 0 100 1000`
do
./ijk.out $i
done >>ijk.txt

for i in `seq 0 100 1000`
do
./ikj.out $i
done >>ikj.txt

for i in `seq 0 100 1000`
do
./jik.out $i
done >>jik.txt

for i in `seq 0 100 1000`
do
./jki.out $i
done >>jki.txt

for i in `seq 0 100 1000`
do
./kij.out $i
done >>kij.txt

for i in `seq 0 100 1000`
do
./kji.out $i
done >>kji.txt

gnuplot << EOF
set key left top
set terminal png
set output "Plot.png"
plot "ijk.txt" using 0:1, "ikj.txt" using 0:1, "jik.txt" using 0:1, "jki.txt" using 0:1, "kij.txt" using 0:1, "kji.txt" using 0:1
exit
EOF
