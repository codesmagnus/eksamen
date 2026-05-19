#include "MonsterRegistry.h"
#include <iostream>
#include <iomanip>

MonsterRegistry::MonsterRegistry() {
    populate();
}

void MonsterRegistry::populate() {
    monsters.emplace_back("Hest",              4,   1);
    monsters.emplace_back("Weak Goblin",       4,   2);
    monsters.emplace_back("Strong Goblin",     8,   3);
    monsters.emplace_back("Stronger Goblin",   10,  4);
    monsters.emplace_back("Den staerkeste Goblin", 15, 5);
    monsters.emplace_back("Abe Kongen",        30,  5);
    monsters.emplace_back("Enhjorning",        50,  8);
    monsters.emplace_back("Drage",             100, 10);
}

const std::vector<Monster>& MonsterRegistry::getAll() const { return monsters; }
const Monster& MonsterRegistry::getAt(int index) const { return monsters.at(index); }
int MonsterRegistry::count() const { return static_cast<int>(monsters.size()); }

void MonsterRegistry::printAll() const {
    std::cout << "\n  #   Name                    HP     STR\n";
    std::cout << "  -   ----                    --     ---\n";
    for (int i = 0; i < static_cast<int>(monsters.size()); ++i) {
        std::cout << "  " << (i + 1) << "   ";
        monsters[i].printStats();
    }
    std::cout << "\n";
}