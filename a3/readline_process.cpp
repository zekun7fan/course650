//
// Created by fanzekun on 2021-11-28.
//
#include <iostream>
#include <sstream>
#include <stdlib.h>


int main(int argc, char **argv) {
    while (!std::cin.eof()) {
        std::string input_str;
        std::getline(std::cin, input_str);
        std::cout << input_str << std::endl;
    }
    exit(0);
}

