#include "quadrado.h"
#include <iostream>

Quadrado::Quadrado(int x, int y) : Figura(x, y) {};
Quadrado::~Quadrado() {};

void Quadrado::desenhar() const
{
    std::cout << "\u3141" << std::endl;
    return ;
}   