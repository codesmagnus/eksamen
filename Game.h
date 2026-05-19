#pragma once
#include "MonsterRegistry.h"
#include <optional>
#include <memory>

class Character;

class Game {
public:
    Game();
    void run();

private:
    MonsterRegistry registry;

    void showMainMenu();
    void startNewCharacter();
    void runAdventure(Character& character);

    void printTitle() const;
};