#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main()
{

	std::ifstream ifs("./words.txt");
	int buf_size = 81;
	char str[buf_size];
	int line_num = 0;
	std::vector<std::string> dictionary;
	std::string word;
	if(ifs.fail()){
		std::cout << "failed to read file" << std::endl;
		return -1;
	}
	while (ifs.getline(str, buf_size)){
		word = str;
		std::sort(word.begin(), word.end());
		do{	
			dictionary.push_back(word);
		}while(std::next_permutation(word.begin(), word.end()));
		++ line_num;
	}
	std::cout << line_num << " " << dictionary.size() << std::endl;
	for (auto word : dictionary){
		std::cout << word << std::endl;
	}


	return 0;
}
