#pragma once
#include "MonsterRegistry.h"
#include "Database.h"
#include <memory>

class Character;

class Game {
public:
    Game();
    void run();

private:
    MonsterRegistry registry;
    Database db;

    void showMainMenu();
    void startNewCharacter();
    void loadExistingCharacter();
    void showStatistics();
    void runAdventure(Character& character);

    void printTitle() const;
};