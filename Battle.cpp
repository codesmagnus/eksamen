#include "Battle.h"
#include "InputHelper.h"
#include <iostream>
#include <cstdlib>

Battle::Battle(Character& player, const MonsterRegistry& registry)
    : player(player), registry(registry) {}

void Battle::printSeparator() const {
    std::cout << "  ----------------------------------------\n";
}

bool Battle::run() {
    while (true) {
        printSeparator();
        std::cout << "\n  What would you like to do?\n"
                  << "  1. Fight a wild monster\n"
                  << "  2. Enter a cave\n"
                  << "  3. View your monsters and items\n"
                  << "  4. Give item to monster\n"
                  << "  5. Quit to main menu\n\n"
                  << "  > ";

        int choice = getIntInput(1, 5);

        if (choice == 5) return false;

        if (choice == 3) {
            player.printRoster();
            for (int i = 0; i < player.monsterCount(); ++i) {
                Monster& m = player.getMonster(i);
                std::cout << "  " << m.getName() << " items:\n";
                m.printItems();
            }
            continue;
        }

        if (choice == 4) {
            // Give item from player inventory to a monster
            if (!player.hasItems()) {
                std::cout << "\n  You have no items to give.\n";
                continue;
            }
            player.printItems();
            std::cout << "  Choose item: ";
            int itemIdx = getIntInput(1, player.itemCount()) - 1;
            player.printRoster();
            std::cout << "  Choose monster to give it to: ";
            int monIdx = getIntInput(1, player.monsterCount()) - 1;
            Monster& m = player.getMonster(monIdx);
            m.addItem(player.takeItem(itemIdx));
            std::cout << "  Item given to " << m.getName() << "!\n";
            continue;
        }

        if (choice == 2) {
            runCave();
            continue;
        }

        // Wild monster fight
        Monster* enemy = chooseEnemy();
        if (!enemy) continue;

        std::cout << "\n  Choose your monster to fight with:\n";
        player.printRoster();
        int monsterIdx = choosePlayerMonster();
        if (monsterIdx < 0) { delete enemy; continue; }

        Monster& myMonster = player.getMonster(monsterIdx);
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
            }
            std::cout << "  You have " << player.monsterCount() << " monster(s) remaining.\n";
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
    return new Monster(registry.getAt(choice - 1));
}

int Battle::choosePlayerMonster() {
    std::cout << "  " << (player.monsterCount() + 1) << ". Go back\n\n  > ";
    int choice = getIntInput(1, player.monsterCount() + 1);
    if (choice == player.monsterCount() + 1) return -1;
    return choice - 1;
}

bool Battle::offerItemUse(Monster& playerMonster, Monster& enemyMonster) {
    if (!playerMonster.hasItems()) return false;

    std::cout << "  Use an item instead of attacking? (y/n): ";
    char c; std::cin >> c;
    if (c != 'y' && c != 'Y') return false;

    std::cout << "\n  " << playerMonster.getName() << "'s items:\n";
    playerMonster.printItems();
    std::cout << "  " << (playerMonster.itemCount() + 1) << ". Cancel\n  > ";
    int choice = getIntInput(1, playerMonster.itemCount() + 1);
    if (choice == playerMonster.itemCount() + 1) return false;

    const Item& item = playerMonster.getItem(choice - 1);
    int dmg = item.use(playerMonster, enemyMonster);
    applyCursedEffect(playerMonster, enemyMonster, dmg);
    return true;
}

void Battle::applyCursedEffect(Monster& attacker, Monster& target, int damageDealt) {
    if (damageDealt > 0 && attacker.hasStatus(StatusType::Cursed)) {
        int curseDmg = damageDealt / 2;
        if (curseDmg < 1) curseDmg = 1;
        std::cout << "  " << attacker.getName() << " is Cursed and takes "
                  << curseDmg << " damage!\n";
        attacker.takeDamage(curseDmg);
    }
    (void)target;
}

bool Battle::fight(Monster& playerMonster, Monster& enemyMonster) {
    std::cout << "\n";
    printSeparator();
    std::cout << "  BATTLE: " << playerMonster.getName()
              << " vs " << enemyMonster.getName() << "\n";
    printSeparator();

    bool playerFirst = (std::rand() % 2 == 0);
    std::cout << "  " << (playerFirst ? playerMonster.getName() : enemyMonster.getName())
              << " attacks first!\n\n";

    while (playerMonster.isAlive() && enemyMonster.isAlive()) {

        // --- Player's turn ---
        std::cout << "\n  -- " << playerMonster.getName() << "'s turn --\n";
        playerMonster.tickStatuses(enemyMonster);
        if (!playerMonster.isAlive()) break;

        if (playerFirst) {
            if (playerMonster.canAct()) {
                playerMonster.consumeTurnStatus();
                bool usedItem = offerItemUse(playerMonster, enemyMonster);
                if (!usedItem) {
                    int dmg = playerMonster.getStrength();
                    enemyMonster.takeDamage(dmg);
                    std::cout << "  " << playerMonster.getName() << " attacks "
                              << enemyMonster.getName() << " for " << dmg << " damage. ("
                              << enemyMonster.getName() << " HP: "
                              << enemyMonster.getHp() << "/" << enemyMonster.getMaxHp() << ")\n";
                    applyCursedEffect(playerMonster, enemyMonster, dmg);
                }
            } else {
                playerMonster.consumeTurnStatus();
                std::cout << "  " << playerMonster.getName() << " cannot act!\n";
            }
            if (!enemyMonster.isAlive()) break;
        }

        // --- Enemy's turn ---
        std::cout << "\n  -- " << enemyMonster.getName() << "'s turn --\n";
        enemyMonster.tickStatuses(playerMonster);
        if (!enemyMonster.isAlive()) break;

        if (enemyMonster.canAct()) {
            enemyMonster.consumeTurnStatus();
            int dmg = enemyMonster.getStrength();
            playerMonster.takeDamage(dmg);
            std::cout << "  " << enemyMonster.getName() << " attacks "
                      << playerMonster.getName() << " for " << dmg << " damage. ("
                      << playerMonster.getName() << " HP: "
                      << playerMonster.getHp() << "/" << playerMonster.getMaxHp() << ")\n";
            applyCursedEffect(enemyMonster, playerMonster, dmg);
        } else {
            enemyMonster.consumeTurnStatus();
            std::cout << "  " << enemyMonster.getName() << " cannot act!\n";
        }

        if (!playerFirst && playerMonster.isAlive()) {
            // Player attacks second
            std::cout << "\n  -- " << playerMonster.getName() << "'s turn --\n";
            if (playerMonster.canAct()) {
                playerMonster.consumeTurnStatus();
                bool usedItem = offerItemUse(playerMonster, enemyMonster);
                if (!usedItem) {
                    int dmg = playerMonster.getStrength();
                    enemyMonster.takeDamage(dmg);
                    std::cout << "  " << playerMonster.getName() << " attacks "
                              << enemyMonster.getName() << " for " << dmg << " damage. ("
                              << enemyMonster.getName() << " HP: "
                              << enemyMonster.getHp() << "/" << enemyMonster.getMaxHp() << ")\n";
                    applyCursedEffect(playerMonster, enemyMonster, dmg);
                }
            } else {
                playerMonster.consumeTurnStatus();
                std::cout << "  " << playerMonster.getName() << " cannot act!\n";
            }
        }
    }

    std::cout << "\n";
    return playerMonster.isAlive();
}

void Battle::offerCapture(const Monster& defeated) {
    Monster capturedCopy(defeated);
    if (player.hasFreeSlot()) {
        std::cout << "  Add " << capturedCopy.getName() << " to your roster? (y/n): ";
        char c; std::cin >> c;
        if (c == 'y' || c == 'Y') {
            player.addMonster(capturedCopy);
            std::cout << "  " << capturedCopy.getName() << " joined your team!\n";
        }
    } else {
        std::cout << "  Roster full. Swap " << capturedCopy.getName()
                  << " with one of your monsters? (y/n): ";
        char c; std::cin >> c;
        if (c == 'y' || c == 'Y') {
            player.printRoster();
            std::cout << "  > ";
            int slot = getIntInput(1, MAX_MONSTERS) - 1;
            std::string old = player.getMonster(slot).getName();
            player.swapMonster(slot, capturedCopy);
            std::cout << "  " << old << " released. " << capturedCopy.getName() << " joined!\n";
        }
    }
}

void Battle::runCave() {
    // Calculate player level from average monster strength
    int totalStr = 0;
    for (int i = 0; i < player.monsterCount(); ++i)
        totalStr += player.getMonster(i).getStrength();
    int avgStr = (player.monsterCount() > 0) ? totalStr / player.monsterCount() : 1;

    Cave cave = Cave::generate(avgStr);
    std::cout << "\n  Entering: " << cave.getName() << "!\n";
    cave.printMonsters();
    std::cout << "  Reward for completing: " << cave.getReward().getName()
              << " - " << cave.getReward().getDescription() << "\n\n";

    int caveMonsterIdx = 0;

    while (!cave.allDefeated() && player.hasMonsters()) {
        // Find next alive cave monster
        while (caveMonsterIdx < cave.monsterCount() && !cave.getMonster(caveMonsterIdx).isAlive())
            caveMonsterIdx++;

        if (caveMonsterIdx >= cave.monsterCount()) break;

        Monster& enemy = cave.getMonster(caveMonsterIdx);
        std::cout << "\n  Next enemy: " << enemy.getName() << " (HP: "
                  << enemy.getHp() << "/" << enemy.getMaxHp() << ")\n";

        std::cout << "  Choose your monster:\n";
        player.printRoster();
        int monIdx = choosePlayerMonster();
        if (monIdx < 0) {
            std::cout << "  You fled the cave!\n";
            return;
        }

        Monster& myMonster = player.getMonster(monIdx);
        bool won = fight(myMonster, enemy);

        if (won) {
            std::cout << "  " << myMonster.getName() << " defeated " << enemy.getName() << "!\n";
            caveMonsterIdx++;
        } else {
            std::cout << "  " << myMonster.getName() << " was defeated!\n";
            player.removeDefeatedMonsters();
            if (!player.hasMonsters()) {
                std::cout << "  All your monsters are defeated! Cave failed.\n";
                return;
            }
        }
    }

    if (cave.allDefeated()) {
        std::cout << "\n  Cave cleared! You receive: " << cave.getReward().getName() << "!\n";
        player.addItem(cave.getReward());
        std::cout << "  The item has been added to your inventory.\n"
                  << "  Use 'Give item to monster' from the menu to equip it.\n";
    }
}

void Battle::giveItemToMonster(const Item& item) {
    player.printRoster();
    std::cout << "  Choose a monster to give " << item.getName() << " to: ";
    int idx = getIntInput(1, player.monsterCount()) - 1;
    player.getMonster(idx).addItem(item);
    std::cout << "  " << item.getName() << " given to "
              << player.getMonster(idx).getName() << "!\n";
}