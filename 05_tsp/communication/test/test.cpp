#include "../chromosome.hpp"
#include <array>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> vec;
    std::cout << sizeof(std::vector<int>) << std::endl;

    vec.resize(20);
    std::cout << sizeof(std::vector<int>) << std::endl;
    std::cout << sizeof(std::array<int, 5>) << std::endl;
    std::cout << sizeof(std::array<int, 20>) << std::endl;
    std::cout << sizeof(Chromosome<128>) << std::endl;
    std::cout << sizeof(Chromosome<512>) << std::endl;
    std::cout << sizeof(Chromosome<2048>) << std::endl;
}
