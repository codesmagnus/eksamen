#include "Character.h"
#include <iostream>
#include <algorithm>

Character::Character(const std::string& name) : name(name) {}

const std::string& Character::getName() const { return name; }

bool Character::addMonster(const Monster& monster) {
    if ((int)roster.size() >= MAX_MONSTERS) return false;
    roster.push_back(monster);
    return true;
}

bool Character::swapMonster(int slot, const Monster& newMonster) {
    if (slot < 0 || slot >= (int)roster.size()) return false;
    roster[slot] = newMonster;
    return true;
}

void Character::removeDefeatedMonsters() {
    roster.erase(
        std::remove_if(roster.begin(), roster.end(),
            [](const Monster& m) { return !m.isAlive(); }),
        roster.end());
}

int Character::monsterCount() const { return (int)roster.size(); }
bool Character::hasMonsters() const { return !roster.empty(); }
bool Character::hasFreeSlot() const { return (int)roster.size() < MAX_MONSTERS; }

Monster& Character::getMonster(int index) { return roster.at(index); }
const Monster& Character::getMonster(int index) const { return roster.at(index); }

void Character::printRoster() const {
    std::cout << "\n  " << name << "'s monsters:\n";
    std::cout << "  #   Name                    HP     STR  Statuses\n";
    std::cout << "  -   ----                    --     ---  --------\n";
    for (int i = 0; i < (int)roster.size(); ++i) {
        std::cout << "  " << (i + 1) << "   ";
        roster[i].printStats();
    }
    std::cout << "\n";
}

void Character::healAllMonsters() {
    for (auto& m : roster) m.healToFull();
}

void Character::addItem(const Item& item) { inventory.push_back(item); }
bool Character::hasItems() const { return !inventory.empty(); }
int Character::itemCount() const { return (int)inventory.size(); }

Item Character::takeItem(int index) {
    Item item = inventory.at(index);
    inventory.erase(inventory.begin() + index);
    return item;
}

void Character::printItems() const {
    if (inventory.empty()) {
        std::cout << "  No items in inventory.\n";
        return;
    }
    std::cout << "\n  " << name << "'s items:\n";
    for (int i = 0; i < (int)inventory.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << inventory[i].getName()
                  << " - " << inventory[i].getDescription() << "\n";
    std::cout << "\n";
}