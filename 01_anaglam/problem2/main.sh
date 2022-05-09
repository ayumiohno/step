#!/bin/bash
read -p "Press file kind (small, medium, large) : " file_kind
if [ -e ${file_kind}_answer.txt ]; then
	rm ${file_kind}_answer.txt
fi
g++ main.cpp
./a.out <<EOS > ${file_kind}_answer.txt
$file_kind.txt
EOS
python3 score_checker.py $file_kind.txt ${file_kind}_answer.txt
