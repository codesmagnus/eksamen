#include "Battle.h"
#include "InputHelper.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

Battle::Battle(Character& player, const MonsterRegistry& registry)
    : player(player), registry(registry) {}

void Battle::printSeparator() const {
    std::cout << "  ----------------------------------------\n";
}

bool Battle::run() {
    while (true) {
        printSeparator();
        std::cout << "\n  What would you like to do?\n"
                  << "  1. Fight a monster\n"
                  << "  2. Quit to main menu\n\n"
                  << "  > ";

        int choice = getIntInput(1, 2);
        if (choice == 2) return false;

        // Pick an enemy
        Monster* enemy = chooseEnemy();
        if (!enemy) continue; // player went back

        // Pick player's monster
        std::cout << "\n  Choose your monster to fight with:\n";
        player.printRoster();
        int monsterIdx = choosePlayerMonster();
        if (monsterIdx < 0) {
            delete enemy;
            continue;
        }

        Monster& myMonster = player.getMonster(monsterIdx);

        // Run the actual fight
        bool playerWon = fight(myMonster, *enemy);

        if (playerWon) {
            std::cout << "\n  Victory! " << myMonster.getName()
                      << " defeated " << enemy->getName() << "!\n";
            offerCapture(*enemy);
        } else {
            std::cout << "\n  " << myMonster.getName() << " was defeated!\n";
            player.removeDefeatedMonsters();

            if (!player.hasMonsters()) {
                std::cout << "\n  All your monsters have been defeated. Returning to main menu...\n";
                delete enemy;
                return false;
            } else {
                std::cout << "  You have " << player.monsterCount() << " monster(s) remaining.\n";
            }
        }

        delete enemy;
    }
}

Monster* Battle::chooseEnemy() {
    std::cout << "\n  Choose an enemy to fight:\n";
    registry.printAll();
    std::cout << "  " << (registry.count() + 1) << ". Go back\n\n  > ";

    int choice = getIntInput(1, registry.count() + 1);
    if (choice == registry.count() + 1) return nullptr;

    // Return a heap-allocated copy so it has its own hp during combat
    return new Monster(registry.getAt(choice - 1));
}

int Battle::choosePlayerMonster() {
    std::cout << "  " << (player.monsterCount() + 1) << ". Go back\n\n  > ";
    int choice = getIntInput(1, player.monsterCount() + 1);
    if (choice == player.monsterCount() + 1) return -1;
    return choice - 1;
}

bool Battle::fight(Monster& playerMonster, Monster& enemyMonster) {
    std::cout << "\n";
    printSeparator();
    std::cout << "  BATTLE: " << playerMonster.getName()
              << " vs " << enemyMonster.getName() << "\n";
    printSeparator();

    // Randomly decide who attacks first
    bool playerFirst = (std::rand() % 2 == 0);
    std::cout << "  " << (playerFirst ? playerMonster.getName() : enemyMonster.getName())
              << " attacks first!\n\n";

    while (playerMonster.isAlive() && enemyMonster.isAlive()) {
        if (playerFirst) {
            // Player attacks
            enemyMonster.takeDamage(playerMonster.getStrength());
            std::cout << "  " << playerMonster.getName() << " attacks "
                      << enemyMonster.getName() << " for "
                      << playerMonster.getStrength() << " damage. ("
                      << enemyMonster.getName() << " HP: "
                      << enemyMonster.getHp() << "/" << enemyMonster.getMaxHp() << ")\n";

            if (!enemyMonster.isAlive()) break;

            // Enemy attacks
            playerMonster.takeDamage(enemyMonster.getStrength());
            std::cout << "  " << enemyMonster.getName() << " attacks "
                      << playerMonster.getName() << " for "
                      << enemyMonster.getStrength() << " damage. ("
                      << playerMonster.getName() << " HP: "
                      << playerMonster.getHp() << "/" << playerMonster.getMaxHp() << ")\n";
        } else {
            // Enemy attacks first
            playerMonster.takeDamage(enemyMonster.getStrength());
            std::cout << "  " << enemyMonster.getName() << " attacks "
                      << playerMonster.getName() << " for "
                      << enemyMonster.getStrength() << " damage. ("
                      << playerMonster.getName() << " HP: "
                      << playerMonster.getHp() << "/" << playerMonster.getMaxHp() << ")\n";

            if (!playerMonster.isAlive()) break;

            // Player attacks
            enemyMonster.takeDamage(playerMonster.getStrength());
            std::cout << "  " << playerMonster.getName() << " attacks "
                      << enemyMonster.getName() << " for "
                      << playerMonster.getStrength() << " damage. ("
                      << enemyMonster.getName() << " HP: "
                      << enemyMonster.getHp() << "/" << enemyMonster.getMaxHp() << ")\n";
        }
    }

    std::cout << "\n";
    return playerMonster.isAlive();
}

void Battle::offerCapture(const Monster& defeated) {
    // Fresh copy with full HP
    Monster capturedCopy(defeated);

    if (player.hasFreeSlot()) {
        std::cout << "  Would you like to add " << capturedCopy.getName()
                  << " to your roster? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            player.addMonster(capturedCopy);
            std::cout << "  " << capturedCopy.getName() << " joined your team!\n";
        }
    } else {
        std::cout << "  Your roster is full (" << MAX_MONSTERS << "/" << MAX_MONSTERS << ").\n"
                  << "  Would you like to swap " << capturedCopy.getName()
                  << " with one of your monsters? (y/n): ";
        char choice;
        std::cin >> choice;
        if (choice == 'y' || choice == 'Y') {
            std::cout << "\n  Choose a monster to replace:\n";
            player.printRoster();
            std::cout << "  > ";
            int slot = getIntInput(1, MAX_MONSTERS) - 1;
            std::string old = player.getMonster(slot).getName();
            player.swapMonster(slot, capturedCopy);
            std::cout << "  " << old << " was released. " << capturedCopy.getName()
                      << " joined your team!\n";
        }
    }
}