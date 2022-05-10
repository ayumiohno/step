#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

int main()
{
	std::ifstream dic("./testcase/words.txt");
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
		std::string word_origin = str;
		std::sort(word_sort.begin(), word_sort.end());
		dictionary.push_back({word_origin, word_sort});
	}

	std::sort(dictionary.begin(), dictionary.end(), [](auto& a, auto& b){return a.second < b.second;});

	std::string file_name;
	std::cin >> file_name;
	std::ifstream words("./" + file_name);
	if(words.fail()){
		std::cout << "failed to read file" << std::endl;
		return -1;
	}
	while (words.getline(str, buf_size)){
		std::string word_sort = str;
		std::string word_origin = str;
		std::sort(word_sort.begin(), word_sort.end());
		auto itr_lower = std::lower_bound(
				dictionary.begin(),
				dictionary.end(), 
				std::pair<std::string, std::string>{"hoge", word_sort},
				[](auto& a, auto& b){return a.second < b.second;});
		auto itr_upper = std::upper_bound(
				dictionary.begin(),
				dictionary.end(), 
				std::pair<std::string, std::string>{"hoge", word_sort},
				[](auto& a, auto& b){return a.second < b.second;});
		if (itr_lower == itr_upper){
			std::cout << "Could not find anaglam" << std::endl;
		} else {
			for(auto itr = itr_lower; itr < itr_upper; ++itr ){
				if ((*itr).first != word_origin){
					std::cout << (*itr).first << " ";
				} else {
					std::cout << "(" + (*itr).first + ")" << " ";
				}
			}
			std::cout << std::endl;
		}
	}

	return 0;
}
