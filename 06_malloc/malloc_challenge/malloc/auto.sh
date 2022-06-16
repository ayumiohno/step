cp freelist.cpp malloc.c
gcc -Wall -o malloc_challenge.bin main.c malloc.c simple_malloc.c -lm
rm result3.txt
make run_trace > result3.txt
#cp malloc_save.c malloc.c
#gcc -Wall -o malloc_challenge.bin main.c malloc.c simple_malloc.c -lm
#rm result4.txt
#make run_trace > result4.txt
