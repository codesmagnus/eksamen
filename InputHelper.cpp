#include "InputHelper.h"
#include <iostream>
#include <limits>
#include <string>
#include <algorithm>

int getIntInput(int min, int max) {
    int value;
    while (true) {
        if (std::cin >> value && value >= min && value <= max) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid input. Please enter a number between "
                  << min << " and " << max << ": ";
    }
}

std::string getLineInput() {
    std::string line;
    while (true) {
        std::getline(std::cin, line);
        // Trim leading/trailing whitespace
        auto start = line.find_first_not_of(" \t\r\n");
        if (start != std::string::npos) {
            auto end = line.find_last_not_of(" \t\r\n");
            return line.substr(start, end - start + 1);
        }
        std::cout << "  Name cannot be empty. Try again: ";
    }
}