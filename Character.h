#pragma once
#include "Monster.h"
#include <string>
#include <vector>
#include <optional>

constexpr int MAX_MONSTERS = 4;

class Character {
public:
    explicit Character(const std::string& name);

    const std::string& getName() const;

    bool addMonster(const Monster& monster);
    bool swapMonster(int slot, const Monster& newMonster);
    void removeDefeatedMonsters();

    int monsterCount() const;
    bool hasMonsters() const;
    bool hasFreeSlot() const;

    Monster& getMonster(int index);
    const Monster& getMonster(int index) const;

    void printRoster() const;
    void healAllMonsters();

private:
    std::string name;
    std::vector<Monster> roster; // up to MAX_MONSTERS
};