#pragma once
#include "Character.h"
#include "MonsterRegistry.h"
#include "Cave.h"

class Battle {
public:
    Battle(Character& player, const MonsterRegistry& registry);

    bool run(); // Returns false to go back to main menu

private:
    Character& player;
    const MonsterRegistry& registry;

    // Wild monster fight (original)
    Monster* chooseEnemy();
    int choosePlayerMonster();
    bool fight(Monster& playerMonster, Monster& enemyMonster);
    void offerCapture(const Monster& defeated);

    // Cave fight
    void runCave();

    // Item usage during battle
    // Returns true if player used an item (skips normal attack)
    bool offerItemUse(Monster& playerMonster, Monster& enemyMonster);

    // Give item to a monster after cave reward
    void giveItemToMonster(const Item& item);

    // Apply cursed effect when a monster deals damage
    void applyCursedEffect(Monster& attacker, Monster& target, int damageDealt);

    void printSeparator() const;
};