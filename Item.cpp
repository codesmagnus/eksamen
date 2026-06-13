#include "Item.h"
#include "Monster.h"
#include <iostream>
#include <cstdlib>

Item::Item(const std::string& name, const std::string& description, const ItemEffect& effect)
    : name(name), description(description), effect(effect) {}

const std::string& Item::getName() const { return name; }
const std::string& Item::getDescription() const { return description; }
const ItemEffect& Item::getEffect() const { return effect; }

int Item::use(Monster& user, Monster& target) const {
    int dmg = effect.damage;

    // Apply direct damage
    if (dmg > 0) {
        target.takeDamage(dmg);
        std::cout << "  " << user.getName() << " used " << name
                  << " on " << target.getName() << " for " << dmg << " damage!\n";
    } else {
        std::cout << "  " << user.getName() << " used " << name << "!\n";
    }

    // Determine status application
    if (effect.status.has_value()) {
        float chance = effect.statusChance;

        // Conditional: Curse - if enemy already has a status, 100% else 20%
        if (effect.conditionalOnEnemyHasStatus) {
            chance = target.hasAnyStatus() ? 1.0f : 0.2f;
        }

        // Conditional: Poison - if enemy has taken damage (hp < maxHp), 100% else 20%
        if (effect.conditionalOnEnemyStatus) {
            chance = (target.getHp() < target.getMaxHp()) ? 1.0f : 0.2f;
        }

        float roll = static_cast<float>(std::rand()) / RAND_MAX;
        if (roll < chance) {
            Monster& statusTarget = effect.statusToSelf ? user : target;
            int turns = -1;
            if (effect.status.value() == StatusType::Frozen) {
                turns = 1 + std::rand() % 2; // 1-2 turns
            } else if (effect.status.value() == StatusType::Stunned) {
                turns = 1;
            }
            statusTarget.addStatus(Status(effect.status.value(), turns));
            std::cout << "  " << statusTarget.getName() << " is now "
                      << Status(effect.status.value()).getName() << "!\n";
        }
    }

    return dmg;
}

namespace Items {
    Item makeBomb() {
        ItemEffect e; e.damage = 10;
        return Item("Bombe", "Deals 10 damage", e);
    }
    Item makeFireBomb() {
        ItemEffect e; e.damage = 5; e.status = StatusType::Stunned; e.statusChance = 0.35f;
        return Item("Ildbombe", "Deals 5 damage, 35% chance to Stun", e);
    }
    Item makeThunderBomb() {
        ItemEffect e; e.damage = 10; e.status = StatusType::Paralyzed; e.statusChance = 0.5f;
        return Item("Tordenbombe", "Deals 10 damage, 50% chance to Paralyze", e);
    }
    Item makeClub() {
        ItemEffect e; e.damage = 20;
        return Item("Kolle", "Deals 20 damage", e);
    }
    Item makeFan() {
        ItemEffect e; e.status = StatusType::Frozen; e.statusChance = 0.8f;
        return Item("Blaaser", "80% chance to Freeze enemy for 1-2 turns", e);
    }
    Item makeCurse() {
        ItemEffect e; e.status = StatusType::Cursed; e.conditionalOnEnemyHasStatus = true;
        return Item("Forbandelse", "100% Curse if enemy has a status, else 20%", e);
    }
    Item makePoison() {
        ItemEffect e; e.status = StatusType::Poisoned; e.conditionalOnEnemyStatus = true;
        return Item("Gift", "100% Poison if enemy took damage, else 20%", e);
    }
}