#include "Cave.h"
#include <iostream>
#include <cstdlib>

Cave::Cave(const std::string& name, std::vector<Monster> monsters, Item reward)
    : name(name), monsters(std::move(monsters)), reward(std::move(reward)) {}

const std::string& Cave::getName() const { return name; }
int Cave::monsterCount() const { return (int)monsters.size(); }
Monster& Cave::getMonster(int index) { return monsters.at(index); }

bool Cave::allDefeated() const {
    for (auto& m : monsters)
        if (m.isAlive()) return false;
    return true;
}

void Cave::printMonsters() const {
    std::cout << "\n  Monsters in " << name << ":\n";
    std::cout << "  #   Name                    HP     STR\n";
    std::cout << "  -   ----                    --     ---\n";
    for (int i = 0; i < (int)monsters.size(); ++i) {
        std::cout << "  " << (i + 1) << "   ";
        monsters[i].printStats();
    }
    std::cout << "\n";
}

const Item& Cave::getReward() const { return reward; }

Cave Cave::generate(int playerLevel) {
    // Scale cave monsters to player level (playerLevel = avg STR of player monsters)
    int baseHp  = 5  + playerLevel * 4;
    int baseStr = 1  + playerLevel;
    int count   = 2  + std::rand() % 3; // 2-4 monsters

    static const char* caveNames[] = {
        "Goblin Hulen", "Drage Grotten", "Abekongens Hule",
        "Den Morke Grotte", "Trollenes Tilholdssted"
    };
    std::string caveName = caveNames[std::rand() % 5];

    static const char* monsterNames[] = {
        "Hule Troll", "Mork Goblin", "Sten Vagt", "Grotte Slange", "Hule Bjorn"
    };

    std::vector<Monster> caveMonsters;
    for (int i = 0; i < count; ++i) {
        int hp  = baseHp  + std::rand() % 6 - 2;
        int str = baseStr + std::rand() % 3 - 1;
        if (hp  < 1) hp  = 1;
        if (str < 1) str = 1;
        caveMonsters.emplace_back(monsterNames[std::rand() % 5], hp, str);
    }

    // Reward: random item
    Item rewards[] = {
        Items::makeBomb(), Items::makeFireBomb(), Items::makeThunderBomb(),
        Items::makeClub(), Items::makeFan(), Items::makeCurse(), Items::makePoison()
    };
    Item reward = rewards[std::rand() % 7];

    return Cave(caveName, std::move(caveMonsters), reward);
}