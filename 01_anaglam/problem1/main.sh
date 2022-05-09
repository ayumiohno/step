#!/bin/bash
read -p "Press file number : " file_num
if [ -e testcase/output$file_num.txt ]; then
	rm testcase/output$file_num.txt
fi
if [ ! -e a.out ] || [ a.out -ot main.cpp ]; then
	g++ main.cpp -o a.out
	if [ $? != 0 ]; then
		echo "failed to compile." >&2
		exit 1
	fi
fi
./a.out <<EOS > testcase/output$file_num.txt
testcase/input$file_num.txt
EOS
