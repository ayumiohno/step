#pragma once
#include "point.hpp"
#include <functional>
#include <vector>

struct MacroGene {

    MacroGene() {}
    ~MacroGene() {}

    void inverse()
    {
        std::swap(this->codon1, this->codon2);
    }

    int codon1;
    int codon2;
};
