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

    // Database id (-1 = not yet saved to the database)
    int getId() const;
    void setId(int id);

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
    const Item& getInventoryItem(int index) const; // peek without removing
    Item takeItem(int index); // remove and return item
    void printItems() const;

private:
    int id = -1;
    std::string name;
    std::vector<Monster> roster;
    std::vector<Item> inventory;
};