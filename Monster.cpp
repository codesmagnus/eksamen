#include "Monster.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

Monster::Monster(const std::string& name, int hp, int strength)
    : name(name), hp(hp), maxHp(hp), strength(strength) {}

Monster::Monster(const Monster& other)
    : name(other.name), hp(other.maxHp), maxHp(other.maxHp),
      strength(other.strength), items(other.items) {
    // Statuses are NOT copied — fresh monster starts clean
}

Monster& Monster::operator=(const Monster& other) {
    name     = other.name;
    hp       = other.maxHp;
    maxHp    = other.maxHp;
    strength = other.strength;
    items    = other.items;
    statuses.clear();
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

void Monster::healToFull() { hp = maxHp; }

void Monster::setHp(int newHp) {
    hp = newHp;
    if (hp < 0) hp = 0;
    if (hp > maxHp) hp = maxHp;
}

void Monster::printStats() const {
    std::cout << std::left << std::setw(22) << name
              << "  HP: " << std::setw(4) << hp << "/" << std::setw(4) << maxHp
              << "  STR: " << std::setw(3) << strength;
    if (!statuses.empty()) {
        std::cout << "  [";
        for (int i = 0; i < (int)statuses.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << statuses[i].getName();
        }
        std::cout << "]";
    }
    std::cout << "\n";
}

// --- Status ---

void Monster::addStatus(const Status& s) {
    // Don't stack same status, just refresh
    for (auto& existing : statuses) {
        if (existing.type == s.type) {
            existing.turnsRemaining = s.turnsRemaining;
            return;
        }
    }
    statuses.push_back(s);
}

bool Monster::hasAnyStatus() const { return !statuses.empty(); }

bool Monster::hasStatus(StatusType t) const {
    for (auto& s : statuses)
        if (s.type == t) return true;
    return false;
}

void Monster::removeStatus(StatusType t) {
    statuses.erase(
        std::remove_if(statuses.begin(), statuses.end(),
            [t](const Status& s) { return s.type == t; }),
        statuses.end());
}

void Monster::tickStatuses(Monster& attackTarget) {
    std::vector<StatusType> toRemove;

    for (auto& s : statuses) {
        switch (s.type) {
            case StatusType::Poisoned:
                std::cout << "  " << name << " is Poisoned and takes 3 damage!\n";
                takeDamage(3);
                break;
            case StatusType::Cursed:
                // Cursed damage is handled in Battle when dealing damage
                std::cout << "  " << name << " is Cursed!\n";
                break;
            case StatusType::Frozen:
                std::cout << "  " << name << " is Frozen and cannot act!\n";
                if (s.turnsRemaining > 0) s.turnsRemaining--;
                if (s.turnsRemaining == 0) toRemove.push_back(StatusType::Frozen);
                break;
            case StatusType::Stunned:
                std::cout << "  " << name << " is Stunned and loses their turn!\n";
                toRemove.push_back(StatusType::Stunned);
                break;
            case StatusType::Paralyzed:
                // Handled in canAct()
                break;
        }
    }

    for (auto t : toRemove) removeStatus(t);
    (void)attackTarget;
}

bool Monster::canAct() const {
    if (hasStatus(StatusType::Frozen))   return false;
    if (hasStatus(StatusType::Stunned))  return false;
    if (hasStatus(StatusType::Paralyzed)) {
        // 30% chance to lose turn
        return (std::rand() % 100) >= 30;
    }
    return true;
}

void Monster::consumeTurnStatus() {
    // Tick down paralyzed (has no fixed duration but keep it simple: 3 turns)
    for (auto& s : statuses) {
        if (s.type == StatusType::Paralyzed && s.turnsRemaining > 0) {
            s.turnsRemaining--;
        }
    }
    statuses.erase(
        std::remove_if(statuses.begin(), statuses.end(),
            [](const Status& s) {
                return s.turnsRemaining == 0;
            }),
        statuses.end());
}

void Monster::printStatuses() const {
    if (statuses.empty()) {
        std::cout << "  No active statuses.\n";
        return;
    }
    for (auto& s : statuses)
        std::cout << "  - " << s.getName() << "\n";
}

// --- Items ---

void Monster::addItem(const Item& item) { items.push_back(item); }
bool Monster::hasItems() const { return !items.empty(); }
int Monster::itemCount() const { return (int)items.size(); }

const Item& Monster::getItem(int index) const { return items.at(index); }

void Monster::printItems() const {
    if (items.empty()) {
        std::cout << "  No items.\n";
        return;
    }
    for (int i = 0; i < (int)items.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << items[i].getName()
                  << " - " << items[i].getDescription() << "\n";
}