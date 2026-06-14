#pragma once
#include "Character.h"
#include "MonsterRegistry.h"
#include "Cave.h"
#include "Database.h"

class Battle {
public:
    Battle(Character& player, const MonsterRegistry& registry, Database& db);

    bool run(); // Returns false to go back to main menu

private:
    Character& player;
    const MonsterRegistry& registry;
    Database& db;

    // Wild monster fight (original)
    Monster* chooseEnemy();
    int choosePlayerMonster();
    bool fight(Monster& playerMonster, Monster& enemyMonster);
    void offerCapture(const Monster& defeated);

    // Cave fight
    void runCave();

    // Item usage during battle.
    // Returns true if player used an item (skips normal attack)
    bool offerItemUse(Monster& playerMonster, Monster& enemyMonster);

    // Apply cursed effect when a monster deals damage
    void applyCursedEffect(Monster& attacker, Monster& target, int damageDealt);

    void printSeparator() const;
};