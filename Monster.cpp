#include "Monster.h"
#include <iostream>
#include <iomanip>

Monster::Monster(const std::string& name, int hp, int strength)
    : name(name), hp(hp), maxHp(hp), strength(strength) {}

Monster::Monster(const Monster& other)
    : name(other.name), hp(other.maxHp), maxHp(other.maxHp), strength(other.strength) {}

Monster& Monster::operator=(const Monster& other) {
    name     = other.name;
    hp       = other.maxHp; // restore to full HP on assignment
    maxHp    = other.maxHp;
    strength = other.strength;
    return *this;
}

const std::string& Monster::getName() const { return name; }
int Monster::getHp() const { return hp; }
int Monster::getMaxHp() const { return maxHp; }
int Monster::getStrength() const { return strength; }
bool Monster::isAlive() const { return hp > 0; }

void Monster::takeDamage(int amount) {
    hp -= amount;
    if (hp < 0) hp = 0;
}

void Monster::healToFull() {
    hp = maxHp;
}

void Monster::printStats() const {
    std::cout << std::left << std::setw(22) << name
              << "  HP: " << std::setw(4) << hp << "/" << std::setw(4) << maxHp
              << "  STR: " << strength << "\n";
}