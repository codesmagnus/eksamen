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
    std::cout << "         MONSTER ADVENTURE - v1.0\n";
    std::cout << "  ==========================================\n\n";
}

void Game::run() {
    printTitle();
    showMainMenu();
}

void Game::showMainMenu() {
    while (true) {
        std::cout << "\n  === MAIN MENU ===\n"
                  << "  1. Start new character\n"
                  << "  2. Quit game\n\n"
                  << "  > ";

        int choice = getIntInput(1, 2);
        if (choice == 2) {
            std::cout << "\n  Thanks for playing! Goodbye.\n\n";
            return;
        }

        startNewCharacter();
    }
}

void Game::startNewCharacter() {
    std::cout << "\n  Enter your character's name: ";
    std::string name = getLineInput();

    auto character = std::make_unique<Character>(name);

    // Start with two "Hest" (Horse) as per spec
    const Monster& hest = registry.getAt(0); // index 0 = Hest
    character->addMonster(Monster(hest));
    character->addMonster(Monster(hest));

    std::cout << "\n  Welcome, " << name << "!\n"
              << "  You start with two Hest monsters.\n";
    character->printRoster();

    runAdventure(*character);
}

void Game::runAdventure(Character& character) {
    std::cout << "\n  Your adventure begins, " << character.getName() << "!\n";

    while (character.hasMonsters()) {
        // Heal all monsters between fights (optional quality of life)
        // Per spec monsters keep their HP - do NOT heal between fights
        Battle battle(character, registry);
        bool continueAdventure = battle.run();

        if (!continueAdventure) {
            std::cout << "\n  " << character.getName()
                      << " retreats to the main menu.\n";
            return;
        }
    }

    std::cout << "\n  All monsters defeated! Returning to main menu.\n";
}