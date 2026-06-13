#pragma once
#include "Monster.h"
#include "Item.h"
#include <vector>
#include <string>

class Cave {
public:
    Cave(const std::string& name, std::vector<Monster> monsters, Item reward);

    const std::string& getName() const;
    int monsterCount() const;
    Monster& getMonster(int index);
    bool allDefeated() const;
    void printMonsters() const;
    const Item& getReward() const;

    // Generate a cave scaled to player's average monster strength
    static Cave generate(int playerLevel);

private:
    std::string name;
    std::vector<Monster> monsters;
    Item reward;
};