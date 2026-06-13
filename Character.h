#pragma once
#include "Monster.h"
#include "Item.h"
#include <string>
#include <vector>

constexpr int MAX_MONSTERS = 4;

class Character {
public:
    explicit Character(const std::string& name);

    const std::string& getName() const;

    // Monster management
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

    // Item inventory
    void addItem(const Item& item);
    bool hasItems() const;
    int itemCount() const;
    Item takeItem(int index); // Remove and return item
    void printItems() const;

private:
    std::string name;
    std::vector<Monster> roster;
    std::vector<Item> inventory;
};