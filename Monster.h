#pragma once
#include <string>

class Monster {
public:
    Monster(const std::string& name, int hp, int strength);
    Monster(const Monster& other);            // Copy constructor (resets hp to max)
    Monster& operator=(const Monster& other); // Copy assignment

    const std::string& getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getStrength() const;
    bool isAlive() const;

    void takeDamage(int amount);
    void healToFull();

    void printStats() const;

private:
    std::string name;
    int hp;
    int maxHp;
    int strength;
};