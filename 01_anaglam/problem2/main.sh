#!/bin/bash
read -p "Press file kind (small, medium, large) : " file_kind
input_file=${file_kind}.txt
output_file=${file_kind}_answer.txt
cd testcase
if [ -e $output_file ]; then
	rm $output_file
fi
cd ..
sourse_file=main.cpp
out_file=a.out
if [ ! -e $out_file ] || [ $out_file -ot $sourse_file ]; then 
	g++ $sourse_file -o $out_file
	if [ $? != 0 ]; then
		echo "failed to compile." >&2
		exit 1
	fi
fi
./a.out <<EOS > testcase/$output_file
testcase/$input_file
EOS
cd testcase
python3 score_checker.py $input_file $output_file
cd ..
