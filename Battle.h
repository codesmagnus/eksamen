#pragma once
#include "Character.h"
#include "MonsterRegistry.h"

class Battle {
public:
    Battle(Character& player, const MonsterRegistry& registry);

    // Returns true if player wants to continue adventuring, false to quit
    bool run();

private:
    Character& player;
    const MonsterRegistry& registry;

    // Returns the enemy monster the player chose to fight
    // Returns nullptr if player chose to leave
    Monster* chooseEnemy();

    // Conducts a fight between a player monster and an enemy monster
    // Returns true if player's monster wins
    bool fight(Monster& playerMonster, Monster& enemyMonster);

    // Lets the player choose which of their monsters to send into battle
    // Returns index into player's roster, or -1 if cancelled
    int choosePlayerMonster();

    // Offer the player to add/swap the defeated enemy into their roster
    void offerCapture(const Monster& defeated);

    void printSeparator() const;
};