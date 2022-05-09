#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>

int main()
{

	std::string file_name;
	std::cin >> file_name;
	std::ifstream dic("./words.txt");
	int buf_size = 81;
	char str[buf_size];
	int line_num = 0;
	std::vector<std::pair<std::string, std::array<int, 26>>> dictionary;
	std::string word;
	const std::array<int, 26> score_list = {1, 3, 2, 2, 1, 3, 3, 1, 1, 4, 4, 2, 2, 1, 1, 3, 4, 1, 1, 1, 2, 3, 3, 4, 3, 4};

	if(dic.fail()){
		std::cout << "failed to read file" << std::endl;
		return -1;
	}
	while (dic.getline(str, buf_size)){
		word = str;
		std::array<int, 26> letter_list;
		for(std::size_t i = 0; i < 26; ++i){
			letter_list.at(i) = 0;
		}
		for(auto letter : word){
			++ letter_list.at(int(letter - 'a'));
		}
		dictionary.push_back({str, letter_list});
	}

	std::sort(dictionary.begin(), dictionary.end());

	std::ifstream words("./" + file_name);
	if(words.fail()){
		std::cout << "failed to read file" << std::endl;
		return -1;
	}
	while (words.getline(str, buf_size)){
		word = str;
		std::array<int, 26> letter_list;
		int score_of_max = 0;
		std::string string_of_max;
		for(std::size_t i = 0; i < 26; ++i){
			letter_list.at(i) = 0;
		}
		for(auto letter : word){
			++ letter_list.at(int(letter - 'a'));
		}
		for(auto letter_list_of_dic : dictionary){
			int score = 0;
			for(std::size_t i = 0; i < 26; ++ i){
				if(letter_list.at(i) >= letter_list_of_dic.second.at(i)){
					score += letter_list_of_dic.second.at(i) * score_list.at(i);
				} else {
					score = 0;
					break;
				}
			}
			if(score > score_of_max){
				score_of_max = score;
				string_of_max = letter_list_of_dic.first;
			}
		}
		std::cout << string_of_max << std::endl;
	}

	return 0;
}
