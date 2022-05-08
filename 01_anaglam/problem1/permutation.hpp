#pragma once
#include <algorithm>
#include <functional>
#include <iostream>

//template<class Type>
class Permutation
{
public:
	Permutation(std::string list)
	{
		list_length = list.size();
		this->list_base = list;
		std::sort(this->list_base.begin(), this->list_base.end()/*, std::greater<>()*/);
		this->list = this->list_base;
	}	

	int findDigit(int searching_digit)      //大きい順になっていない桁を見つける
	{
		if(searching_digit == -1  || list.at(searching_digit) < list.at(searching_digit + 1) ){
			return searching_digit;
		} else{
			findDigit(searching_digit - 1);
		}
	}

	bool permutate()
	{

		if(list_length <= 1){
			return false;
		}

		auto starting_digit_of_list = findDigit(list_length - 2);


		if(starting_digit_of_list == -1){
			return false;
		}

		auto next_index_of_base = list_base.rfind(list.at(starting_digit_of_list)) + 1;

		while(true){
			auto next_index_interater_of_list  = std::find(list.begin() + starting_digit_of_list, list.end(), list_base.at(next_index_of_base));
			if(next_index_interater_of_list != list.end()){
				std::swap(list.at(starting_digit_of_list), *(next_index_interater_of_list));
				std::sort(list.begin() + starting_digit_of_list + 1, list.end());
				//std::cout << "after: "<< list << std::endl;
				break;
			}
			++ next_index_of_base;
		}

		return true;
	}

	std::string getList()       //TODO listを参照かポインタで入れて直接編集し、list_baseはコピーしたものを入れたい
	{
		return list;
	}

private:
	std::string list_base;      //初期の配列
	std::string list;           //生成された配列
	int list_length;	 //配列の長さ

};
