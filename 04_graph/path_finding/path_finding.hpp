#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

class NoSuchValueException
{
public:
    explicit NoSuchValueException(std::string val) : value{val} {}

    void printError() const
    {
        std::cout << value << " : no such value!!" << std::endl;
    }

private:
    std::string value;
};

void depthFirstSearch(const std::string& start_value,
    const std::string& goal_value,
    const std::unordered_map<std::string, std::string>& pages,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& links);

void breadthFirstSearch(const std::string& start_value,
    const std::string& goal_value,
    const std::unordered_map<std::string, std::string>& pages,
    const std::unordered_map<std::string, std::unordered_set<std::string>>& links);
