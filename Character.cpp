#include "Character.h"
#include <iostream>
#include <algorithm>

Character::Character(const std::string& name) : name(name) {}

const std::string& Character::getName() const { return name; }

bool Character::addMonster(const Monster& monster) {
    if (static_cast<int>(roster.size()) >= MAX_MONSTERS) return false;
    roster.push_back(monster);
    return true;
}

bool Character::swapMonster(int slot, const Monster& newMonster) {
    if (slot < 0 || slot >= static_cast<int>(roster.size())) return false;
    roster[slot] = newMonster;
    return true;
}

void Character::removeDefeatedMonsters() {
    roster.erase(
        std::remove_if(roster.begin(), roster.end(),
            [](const Monster& m) { return !m.isAlive(); }),
        roster.end()
    );
}

int Character::monsterCount() const { return static_cast<int>(roster.size()); }
bool Character::hasMonsters() const { return !roster.empty(); }
bool Character::hasFreeSlot() const { return static_cast<int>(roster.size()) < MAX_MONSTERS; }

Monster& Character::getMonster(int index) { return roster.at(index); }
const Monster& Character::getMonster(int index) const { return roster.at(index); }

void Character::printRoster() const {
    std::cout << "\n  " << name << "'s monsters:\n";
    std::cout << "  #   Name                    HP     STR\n";
    std::cout << "  -   ----                    --     ---\n";
    for (int i = 0; i < static_cast<int>(roster.size()); ++i) {
        std::cout << "  " << (i + 1) << "   ";
        roster[i].printStats();
    }
    std::cout << "\n";
}

void Character::healAllMonsters() {
    for (auto& m : roster) m.healToFull();
}