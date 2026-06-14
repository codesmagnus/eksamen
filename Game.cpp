#include "Game.h"
#include "Character.h"
#include "Battle.h"
#include "InputHelper.h"
#include <iostream>
#include <memory>

Game::Game() {}

void Game::printTitle() const {
    std::cout << "\n";
    std::cout << "  ==========================================\n";
    std::cout << "         MONSTER ADVENTURE - v3.0\n";
    std::cout << "  ==========================================\n\n";

    if (!db.isOpen()) {
        std::cout << "  WARNING: Database could not be opened.\n"
                  << "  Saving and loading heroes will not work this session.\n\n";
    }
}

void Game::run() {
    printTitle();
    showMainMenu();
}

void Game::showMainMenu() {
    while (true) {
        std::cout << "\n  === MAIN MENU ===\n"
                  << "  1. Start new hero\n"
                  << "  2. Load existing hero\n"
                  << "  3. View statistics\n"
                  << "  4. Quit game\n\n"
                  << "  > ";

        int choice = getIntInput(1, 4);

        if (choice == 4) {
            std::cout << "\n  Thanks for playing! Goodbye.\n\n";
            return;
        }

        if (choice == 1) {
            startNewCharacter();
        } else if (choice == 2) {
            loadExistingCharacter();
        } else if (choice == 3) {
            showStatistics();
        }
    }
}

void Game::startNewCharacter() {
    std::cout << "\n  Enter your hero's name: ";
    std::string name = getLineInput();

    auto character = std::make_unique<Character>(name);

    // Start with two "Hest" (Horse) as per spec
    const Monster& hest = registry.getAt(0); // index 0 = Hest
    character->addMonster(Monster(hest));
    character->addMonster(Monster(hest));

    std::cout << "\n  Welcome, " << name << "!\n"
              << "  You start with two Hest monsters.\n";
    character->printRoster();

    db.saveHero(*character);

    runAdventure(*character);
}

void Game::loadExistingCharacter() {
    auto heroes = db.listHeroes();

    if (heroes.empty()) {
        std::cout << "\n  No saved heroes found yet. Start a new hero first!\n";
        return;
    }

    std::cout << "\n  === SAVED HEROES ===\n";
    for (int i = 0; i < (int)heroes.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << heroes[i].name << "\n";
    }
    std::cout << "  " << (heroes.size() + 1) << ". Go back\n\n  > ";

    int choice = getIntInput(1, (int)heroes.size() + 1);
    if (choice == (int)heroes.size() + 1) return;

    Character character("");
    if (!db.loadHero(heroes[choice - 1].id, character)) {
        std::cout << "\n  Failed to load hero.\n";
        return;
    }

    std::cout << "\n  Welcome back, " << character.getName() << "!\n";
    character.printRoster();

    runAdventure(character);
}

void Game::showStatistics() {
    auto heroes = db.listHeroes();

    std::cout << "\n  === GAME STATISTICS ===\n";
    std::cout << "  Total monsters defeated (all heroes): "
              << db.getTotalMonstersDefeatedGlobal() << "\n";

    if (heroes.empty()) {
        std::cout << "  No heroes saved yet.\n";
        return;
    }

    std::cout << "\n  Choose a hero to see detailed stats:\n";
    for (int i = 0; i < (int)heroes.size(); ++i)
        std::cout << "  " << (i + 1) << ". " << heroes[i].name << "\n";
    std::cout << "  " << (heroes.size() + 1) << ". Go back\n\n  > ";

    int choice = getIntInput(1, (int)heroes.size() + 1);
    if (choice == (int)heroes.size() + 1) return;

    HeroStatistics stats = db.getStatistics(heroes[choice - 1].id);

    std::cout << "\n  --- " << heroes[choice - 1].name << "'s statistics ---\n";
    std::cout << "  Monsters defeated: " << stats.monstersDefeated << "\n";

    std::cout << "\n  Item usage:\n";
    if (stats.itemStats.empty()) {
        std::cout << "    (no items used yet)\n";
    } else {
        for (auto& s : stats.itemStats)
            std::cout << "    " << s.itemName << ": used " << s.timesUsed
                      << " time(s), defeated " << s.monstersDefeated << " monster(s)\n";
    }
    std::cout << "  Most used item: " << stats.mostUsedItem() << "\n";

    std::cout << "\n  Monster usage:\n";
    if (stats.monsterStats.empty()) {
        std::cout << "    (no monsters used yet)\n";
    } else {
        for (auto& s : stats.monsterStats)
            std::cout << "    " << s.monsterName << ": sent into battle "
                      << s.timesUsed << " time(s)\n";
    }
    std::cout << "  Most used monster: " << stats.mostUsedMonster() << "\n";
}

void Game::runAdventure(Character& character) {
    std::cout << "\n  Your adventure begins, " << character.getName() << "!\n";

    while (character.hasMonsters()) {
        Battle battle(character, registry, db);
        bool continueAdventure = battle.run();

        // Persist progress (roster, items, hp) after the session
        db.saveHero(character);

        if (!continueAdventure) {
            std::cout << "\n  " << character.getName()
                      << " retreats to the main menu. Progress saved!\n";
            return;
        }
    }

    std::cout << "\n  All monsters defeated! Returning to main menu.\n";
    db.saveHero(character);
}