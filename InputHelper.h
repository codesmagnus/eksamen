#pragma once
#include <string>

// Reads an integer from stdin in range [min, max] (inclusive).
// Reprompts on bad input.
int getIntInput(int min, int max);

// Reads a non-empty trimmed string from stdin.
std::string getLineInput();