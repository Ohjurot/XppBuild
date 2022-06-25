#include "util.h"

#include <iostream>

int main(int argc, char** argv)
{
    float off, value;
    std::cout << "Enter offset: ";
    std::cin >> off;
    std::cout << "Enter value: ";
    std::cin >> value;

    std::cout << std::endl << "The result is: " << my_util::my_sin(off, value);
}
