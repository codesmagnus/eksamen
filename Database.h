#pragma once
#include <QSqlDatabase>
#include <string>
#include <vector>
#include "Character.h"

// Summary info for a hero, used when listing heroes at game start.
struct HeroSummary {
    int id;
    std::string name;
};

// Usage/defeat statistics for one "Ting" (item), for one hero.
struct ItemStat {
    std::string itemName;
    int timesUsed;
    int monstersDefeated;
};

// Usage statistics for one Monster type, for one hero.
struct MonsterStat {
    std::string monsterName;
    int timesUsed;
};

// Full statistics package for a single hero.
struct HeroStatistics {
    int monstersDefeated = 0;
    std::vector<ItemStat> itemStats;
    std::vector<MonsterStat> monsterStats;

    std::string mostUsedItem() const;
    std::string mostUsedMonster() const;
};

// Database is the single point of access to the SQLite database.
// It is implemented on top of Qt's QSqlDatabase / QSqlQuery using the
// QSQLITE driver (provided by libqt5sql5-sqlite).
class Database {
public:
    // Opens (and creates if necessary) the SQLite database file at `path`.
    explicit Database(const std::string& path = "monster_game.db");
    ~Database();

    bool isOpen() const;

    // Returns all heroes that have been saved so far.
    std::vector<HeroSummary> listHeroes();

    // Saves (inserts or updates) a hero, its roster and its inventory.
    // If the hero has never been saved before (getId() == -1), a new
    // row is created in `heroes` and the new id is written back onto
    // the Character object.
    void saveHero(Character& hero);

    // Loads a hero (including roster, monster items and inventory) by id.
    // Returns true on success.
    bool loadHero(int heroId, Character& outHero);

    // Deletes a hero and all associated data.
    void deleteHero(int heroId);

    // --- Statistics ---

    // Total number of monsters defeated across ALL heroes.
    int getTotalMonstersDefeatedGlobal();

    // Call once whenever a monster is defeated in battle (by this hero).
    void recordMonsterDefeated(int heroId);

    // Call whenever a "Ting" (item) is used in battle.
    // `monsterDefeated` should be true if that exact use of the item
    // finished off the enemy monster.
    void recordItemUsed(int heroId, const std::string& itemName, bool monsterDefeated);

    // Call whenever the player sends a monster into battle.
    void recordMonsterUsed(int heroId, const std::string& monsterName);

    // Returns the aggregated statistics for one hero.
    HeroStatistics getStatistics(int heroId);

private:
    QSqlDatabase db;
    void initTables();
};