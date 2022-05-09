#!/bin/bash
read -p "Press file number : " file_num
if [ -e testcase/output$file_num.txt ]; then
	rm testcase/output$file_num.txt
fi
g++ main.cpp
./a.out <<EOS > testcase/output$file_num.txt
testcase/input$file_num.txt
EOS
