#pragma once
#include "Status.h"
#include <string>
#include <optional>

class Monster;

struct ItemEffect {
    int damage = 0;                        // Direct damage dealt
    std::optional<StatusType> status;      // Status to apply
    float statusChance = 0.0f;            // 0.0 - 1.0 probability
    bool statusToSelf = false;            // Apply status to self instead of enemy
    bool conditionalOnEnemyStatus = false; // Poison: only if enemy took damage
    bool conditionalOnEnemyHasStatus = false; // Curse: higher chance if enemy has status
};

class Item {
public:
    Item(const std::string& name, const std::string& description, const ItemEffect& effect);

    const std::string& getName() const;
    const std::string& getDescription() const;
    const ItemEffect& getEffect() const;

    // Apply this item during battle. Returns damage dealt.
    int use(Monster& user, Monster& target) const;

private:
    std::string name;
    std::string description;
    ItemEffect effect;
};

// Factory functions for all items
namespace Items {
    Item makeBomb();
    Item makeFireBomb();
    Item makeThunderBomb();
    Item makeClub();
    Item makeFan();
    Item makeCurse();
    Item makePoison();

    // Recreate an item by its name (used when loading from the database)
    std::optional<Item> fromName(const std::string& name);
}