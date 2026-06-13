#pragma once
#include <string>

enum class StatusType {
    Poisoned,   // Takes small damage each turn
    Frozen,     // Cannot act for 1-2 turns
    Paralyzed,  // 30% chance to lose turn
    Stunned,    // Guaranteed to miss next turn
    Cursed      // Takes damage when dealing damage
};

struct Status {
    StatusType type;
    int turnsRemaining; // -1 = permanent until cured

    Status(StatusType t, int turns = -1) : type(t), turnsRemaining(turns) {}

    std::string getName() const {
        switch (type) {
            case StatusType::Poisoned:  return "Poisoned";
            case StatusType::Frozen:    return "Frozen";
            case StatusType::Paralyzed: return "Paralyzed";
            case StatusType::Stunned:   return "Stunned";
            case StatusType::Cursed:    return "Cursed";
        }
        return "Unknown";
    }
};