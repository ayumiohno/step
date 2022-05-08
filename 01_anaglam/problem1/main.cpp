#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
//#include "permutation.hpp"

int main()
{

	std::ifstream dic("./words.txt");
	int buf_size = 81;
	char str[buf_size];
	int line_num = 0;
	std::vector<std::string> dictionary;
	std::string word;
	if(dic.fail()){
		std::cout << "failed to read file" << std::endl;
		return -1;
	}
	while (dic.getline(str, buf_size)){
		word = str;
		std::sort(word.begin(), word.end());
		dictionary.push_back(word);

		/* auto permutation = Permutation{word};
		do{
			dictionary.push_back(permutation.getList());
		}while(permutation.permutate());*/

	}

	/*for (auto word : dictionary){
		std::cout << word << std::endl;
	}*/

	std::sort(dictionary.begin(), dictionary.end());

	std::ifstream words("./testcase/words2.txt");
	if(words.fail()){
		std::cout << "failed to read file" << std::endl;
		return -1;
	}
	while (words.getline(str, buf_size)){
		word = str;
		std::sort(word.begin(), word.end());
		//if (std::find(dictionary.begin(), dictionary.end(), word) != dictionary.end()){
		if(*(std::lower_bound(dictionary.begin(), dictionary.end(), word)) == word ){
			std::cout << word << std::endl;
		}
	}

	return 0;
}
