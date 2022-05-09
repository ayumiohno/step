#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main()
{

	std::string file_name;
	std::cin >> file_name;
	std::ifstream dic("./words.txt");
	int buf_size = 81;
	char str[buf_size];
	int line_num = 0;
	std::vector<std::pair<std::string, std::string>> dictionary;
	if(dic.fail()){
		std::cout << "failed to read file" << std::endl;
		return -1;
	}
	while (dic.getline(str, buf_size)){
		std::string word_sort = str;
		std::string word_original = str;
		std::sort(word_sort.begin(), word_sort.end());
		dictionary.push_back({word_original, word_sort});
	}

	std::sort(dictionary.begin(), dictionary.end(), [](auto& a, auto& b){return a.second < b.second;});

	std::ifstream words("./" + file_name);
	if(words.fail()){
		std::cout << "failed to read file" << std::endl;
		return -1;
	}
	while (words.getline(str, buf_size)){
		std::string word;
		word = str;
		std::sort(word.begin(), word.end());
		auto word_of_dic = *std::lower_bound(
				dictionary.begin(),
				dictionary.end(), 
				std::pair<std::string, std::string>{"hoge", word},
				[](auto& a, auto& b){return a.second < b.second;});
		if(word_of_dic.second == word){
			std::cout << word_of_dic.first << std::endl;
		} else {
			std::cout << "Could not find anaglam" << std::endl;
		}
	}

	return 0;
}
