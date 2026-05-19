#pragma once
#include "Monster.h"
#include <vector>

class MonsterRegistry {
public:
    MonsterRegistry();

    const std::vector<Monster>& getAll() const;
    const Monster& getAt(int index) const;
    int count() const;

    void printAll() const;

private:
    std::vector<Monster> monsters;

    void populate();
};