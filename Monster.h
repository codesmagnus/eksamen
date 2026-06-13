#pragma once
#include "Status.h"
#include "Item.h"
#include <string>
#include <vector>

class Monster {
public:
    Monster(const std::string& name, int hp, int strength);
    Monster(const Monster& other);
    Monster& operator=(const Monster& other);

    const std::string& getName() const;
    int getHp() const;
    int getMaxHp() const;
    int getStrength() const;
    bool isAlive() const;

    void takeDamage(int amount);
    void healToFull();
    void printStats() const;

    // Status methods
    void addStatus(const Status& s);
    bool hasAnyStatus() const;
    bool hasStatus(StatusType t) const;
    void removeStatus(StatusType t);
    void tickStatuses(Monster& attackTarget); // Run at start of turn, may damage attackTarget
    void printStatuses() const;

    // Item methods
    void addItem(const Item& item);
    bool hasItems() const;
    int itemCount() const;
    const Item& getItem(int index) const;
    void printItems() const;

    // Returns true if monster can act this turn (not frozen/stunned)
    bool canAct() const;
    // Call after determining monster acts - clears stunned/decrements frozen
    void consumeTurnStatus();

private:
    std::string name;
    int hp;
    int maxHp;
    int strength;

    std::vector<Status> statuses;
    std::vector<Item> items;
};