#include "Database.h"
#include "Item.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QString>
#include <iostream>
#include <algorithm>

// --- HeroStatistics helpers ---

std::string HeroStatistics::mostUsedItem() const {
    if (itemStats.empty()) return "(none)";
    auto best = std::max_element(itemStats.begin(), itemStats.end(),
        [](const ItemStat& a, const ItemStat& b) { return a.timesUsed < b.timesUsed; });
    if (best->timesUsed == 0) return "(none)";
    return best->itemName;
}

std::string HeroStatistics::mostUsedMonster() const {
    if (monsterStats.empty()) return "(none)";
    auto best = std::max_element(monsterStats.begin(), monsterStats.end(),
        [](const MonsterStat& a, const MonsterStat& b) { return a.timesUsed < b.timesUsed; });
    if (best->timesUsed == 0) return "(none)";
    return best->monsterName;
}

// --- Database ---

Database::Database(const std::string& path) {
    // Use a unique connection name in case multiple Database objects exist.
    static int connectionCounter = 0;
    QString connName = QString("monster_game_conn_%1").arg(connectionCounter++);

    db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName(QString::fromStdString(path));

    if (!db.open()) {
        std::cerr << "  [Database] Failed to open database: "
                  << db.lastError().text().toStdString() << "\n";
        return;
    }

    initTables();
}

Database::~Database() {
    if (db.isOpen()) db.close();
}

bool Database::isOpen() const {
    return db.isOpen();
}

void Database::initTables() {
    QSqlQuery q(db);

    q.exec(
        "CREATE TABLE IF NOT EXISTS heroes ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE"
        ")"
    );

    q.exec(
        "CREATE TABLE IF NOT EXISTS hero_monsters ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  hero_id INTEGER NOT NULL,"
        "  slot_index INTEGER NOT NULL,"
        "  monster_name TEXT NOT NULL,"
        "  hp INTEGER NOT NULL,"
        "  max_hp INTEGER NOT NULL,"
        "  strength INTEGER NOT NULL,"
        "  FOREIGN KEY(hero_id) REFERENCES heroes(id)"
        ")"
    );

    q.exec(
        "CREATE TABLE IF NOT EXISTS monster_items ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  hero_monster_id INTEGER NOT NULL,"
        "  item_name TEXT NOT NULL,"
        "  FOREIGN KEY(hero_monster_id) REFERENCES hero_monsters(id)"
        ")"
    );

    q.exec(
        "CREATE TABLE IF NOT EXISTS hero_inventory ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  hero_id INTEGER NOT NULL,"
        "  item_name TEXT NOT NULL,"
        "  FOREIGN KEY(hero_id) REFERENCES heroes(id)"
        ")"
    );

    q.exec(
        "CREATE TABLE IF NOT EXISTS hero_stats ("
        "  hero_id INTEGER PRIMARY KEY,"
        "  monsters_defeated INTEGER NOT NULL DEFAULT 0,"
        "  FOREIGN KEY(hero_id) REFERENCES heroes(id)"
        ")"
    );

    q.exec(
        "CREATE TABLE IF NOT EXISTS item_usage ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  hero_id INTEGER NOT NULL,"
        "  item_name TEXT NOT NULL,"
        "  times_used INTEGER NOT NULL DEFAULT 0,"
        "  monsters_defeated INTEGER NOT NULL DEFAULT 0,"
        "  UNIQUE(hero_id, item_name),"
        "  FOREIGN KEY(hero_id) REFERENCES heroes(id)"
        ")"
    );

    q.exec(
        "CREATE TABLE IF NOT EXISTS monster_usage ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  hero_id INTEGER NOT NULL,"
        "  monster_name TEXT NOT NULL,"
        "  times_used INTEGER NOT NULL DEFAULT 0,"
        "  UNIQUE(hero_id, monster_name),"
        "  FOREIGN KEY(hero_id) REFERENCES heroes(id)"
        ")"
    );
}

std::vector<HeroSummary> Database::listHeroes() {
    std::vector<HeroSummary> result;
    QSqlQuery q(db);
    q.exec("SELECT id, name FROM heroes ORDER BY id");
    while (q.next()) {
        HeroSummary h;
        h.id = q.value(0).toInt();
        h.name = q.value(1).toString().toStdString();
        result.push_back(h);
    }
    return result;
}

void Database::saveHero(Character& hero) {
    if (hero.getId() == -1) {
        QSqlQuery insertHero(db);
        insertHero.prepare("INSERT INTO heroes (name) VALUES (?)");
        insertHero.addBindValue(QString::fromStdString(hero.getName()));
        if (!insertHero.exec()) {
            std::cerr << "  [Database] Failed to insert hero: "
                      << insertHero.lastError().text().toStdString() << "\n";
            return;
        }
        hero.setId(insertHero.lastInsertId().toInt());

        QSqlQuery initStats(db);
        initStats.prepare("INSERT OR IGNORE INTO hero_stats (hero_id, monsters_defeated) VALUES (?, 0)");
        initStats.addBindValue(hero.getId());
        initStats.exec();
    }

    int heroId = hero.getId();

    // Clear existing roster/inventory rows for this hero so we can
    // re-insert the current state (simple "delete then insert" upsert).
    QSqlQuery delItems(db);
    delItems.prepare(
        "DELETE FROM monster_items WHERE hero_monster_id IN "
        "(SELECT id FROM hero_monsters WHERE hero_id = ?)");
    delItems.addBindValue(heroId);
    delItems.exec();

    QSqlQuery delMonsters(db);
    delMonsters.prepare("DELETE FROM hero_monsters WHERE hero_id = ?");
    delMonsters.addBindValue(heroId);
    delMonsters.exec();

    QSqlQuery delInventory(db);
    delInventory.prepare("DELETE FROM hero_inventory WHERE hero_id = ?");
    delInventory.addBindValue(heroId);
    delInventory.exec();

    // Re-insert roster
    for (int i = 0; i < hero.monsterCount(); ++i) {
        const Monster& m = hero.getMonster(i);

        QSqlQuery insMon(db);
        insMon.prepare(
            "INSERT INTO hero_monsters (hero_id, slot_index, monster_name, hp, max_hp, strength) "
            "VALUES (?, ?, ?, ?, ?, ?)");
        insMon.addBindValue(heroId);
        insMon.addBindValue(i);
        insMon.addBindValue(QString::fromStdString(m.getName()));
        insMon.addBindValue(m.getHp());
        insMon.addBindValue(m.getMaxHp());
        insMon.addBindValue(m.getStrength());

        if (!insMon.exec()) {
            std::cerr << "  [Database] Failed to insert monster: "
                      << insMon.lastError().text().toStdString() << "\n";
            continue;
        }

        int monsterRowId = insMon.lastInsertId().toInt();

        for (int j = 0; j < m.itemCount(); ++j) {
            QSqlQuery insItem(db);
            insItem.prepare("INSERT INTO monster_items (hero_monster_id, item_name) VALUES (?, ?)");
            insItem.addBindValue(monsterRowId);
            insItem.addBindValue(QString::fromStdString(m.getItem(j).getName()));
            insItem.exec();
        }
    }

    // Re-insert inventory
    for (int i = 0; i < hero.itemCount(); ++i) {
        QSqlQuery insInv(db);
        insInv.prepare("INSERT INTO hero_inventory (hero_id, item_name) VALUES (?, ?)");
        insInv.addBindValue(heroId);
        insInv.addBindValue(QString::fromStdString(hero.getInventoryItem(i).getName()));
        insInv.exec();
    }
}

bool Database::loadHero(int heroId, Character& outHero) {
    QSqlQuery heroQuery(db);
    heroQuery.prepare("SELECT name FROM heroes WHERE id = ?");
    heroQuery.addBindValue(heroId);
    if (!heroQuery.exec() || !heroQuery.next()) {
        return false;
    }

    outHero = Character(heroQuery.value(0).toString().toStdString());
    outHero.setId(heroId);

    // Load roster, ordered by slot index
    QSqlQuery monQuery(db);
    monQuery.prepare(
        "SELECT id, monster_name, hp, max_hp, strength FROM hero_monsters "
        "WHERE hero_id = ? ORDER BY slot_index");
    monQuery.addBindValue(heroId);
    monQuery.exec();

    while (monQuery.next()) {
        int monsterRowId   = monQuery.value(0).toInt();
        std::string mName  = monQuery.value(1).toString().toStdString();
        int hp             = monQuery.value(2).toInt();
        int maxHp          = monQuery.value(3).toInt();
        int strength       = monQuery.value(4).toInt();

        Monster m(mName, maxHp, strength);

        // Load items equipped on this monster
        QSqlQuery itemQuery(db);
        itemQuery.prepare("SELECT item_name FROM monster_items WHERE hero_monster_id = ?");
        itemQuery.addBindValue(monsterRowId);
        itemQuery.exec();
        while (itemQuery.next()) {
            auto item = Items::fromName(itemQuery.value(0).toString().toStdString());
            if (item) m.addItem(*item);
        }

        // addMonster() copy-constructs, which resets hp to maxHp,
        // so restore the saved current hp afterwards.
        outHero.addMonster(m);
        outHero.getMonster(outHero.monsterCount() - 1).setHp(hp);
    }

    // Load inventory
    QSqlQuery invQuery(db);
    invQuery.prepare("SELECT item_name FROM hero_inventory WHERE hero_id = ?");
    invQuery.addBindValue(heroId);
    invQuery.exec();
    while (invQuery.next()) {
        auto item = Items::fromName(invQuery.value(0).toString().toStdString());
        if (item) outHero.addItem(*item);
    }

    return true;
}

void Database::deleteHero(int heroId) {
    QSqlQuery delItems(db);
    delItems.prepare(
        "DELETE FROM monster_items WHERE hero_monster_id IN "
        "(SELECT id FROM hero_monsters WHERE hero_id = ?)");
    delItems.addBindValue(heroId);
    delItems.exec();

    const char* tables[] = {
        "hero_monsters", "hero_inventory", "hero_stats",
        "item_usage", "monster_usage", "heroes"
    };
    for (const char* table : tables) {
        QSqlQuery del(db);
        QString sql = QString("DELETE FROM %1 WHERE %2 = ?")
            .arg(table)
            .arg(QString(table) == "heroes" ? "id" : "hero_id");
        del.prepare(sql);
        del.addBindValue(heroId);
        del.exec();
    }
}

int Database::getTotalMonstersDefeatedGlobal() {
    QSqlQuery q(db);
    q.exec("SELECT SUM(monsters_defeated) FROM hero_stats");
    if (q.next()) return q.value(0).toInt();
    return 0;
}

void Database::recordMonsterDefeated(int heroId) {
    QSqlQuery q(db);
    q.prepare(
        "INSERT INTO hero_stats (hero_id, monsters_defeated) VALUES (?, 1) "
        "ON CONFLICT(hero_id) DO UPDATE SET monsters_defeated = monsters_defeated + 1");
    q.addBindValue(heroId);
    if (!q.exec()) {
        std::cerr << "  [Database] recordMonsterDefeated failed: "
                  << q.lastError().text().toStdString() << "\n";
    }
}

void Database::recordItemUsed(int heroId, const std::string& itemName, bool monsterDefeated) {
    QSqlQuery q(db);
    q.prepare(
        "INSERT INTO item_usage (hero_id, item_name, times_used, monsters_defeated) "
        "VALUES (?, ?, 1, ?) "
        "ON CONFLICT(hero_id, item_name) DO UPDATE SET "
        "  times_used = times_used + 1, "
        "  monsters_defeated = monsters_defeated + ?");
    q.addBindValue(heroId);
    q.addBindValue(QString::fromStdString(itemName));
    q.addBindValue(monsterDefeated ? 1 : 0);
    q.addBindValue(monsterDefeated ? 1 : 0);
    if (!q.exec()) {
        std::cerr << "  [Database] recordItemUsed failed: "
                  << q.lastError().text().toStdString() << "\n";
    }
}

void Database::recordMonsterUsed(int heroId, const std::string& monsterName) {
    QSqlQuery q(db);
    q.prepare(
        "INSERT INTO monster_usage (hero_id, monster_name, times_used) VALUES (?, ?, 1) "
        "ON CONFLICT(hero_id, monster_name) DO UPDATE SET times_used = times_used + 1");
    q.addBindValue(heroId);
    q.addBindValue(QString::fromStdString(monsterName));
    if (!q.exec()) {
        std::cerr << "  [Database] recordMonsterUsed failed: "
                  << q.lastError().text().toStdString() << "\n";
    }
}

HeroStatistics Database::getStatistics(int heroId) {
    HeroStatistics stats;

    QSqlQuery statQuery(db);
    statQuery.prepare("SELECT monsters_defeated FROM hero_stats WHERE hero_id = ?");
    statQuery.addBindValue(heroId);
    if (statQuery.exec() && statQuery.next()) {
        stats.monstersDefeated = statQuery.value(0).toInt();
    }

    QSqlQuery itemQuery(db);
    itemQuery.prepare(
        "SELECT item_name, times_used, monsters_defeated FROM item_usage "
        "WHERE hero_id = ? ORDER BY item_name");
    itemQuery.addBindValue(heroId);
    itemQuery.exec();
    while (itemQuery.next()) {
        ItemStat s;
        s.itemName = itemQuery.value(0).toString().toStdString();
        s.timesUsed = itemQuery.value(1).toInt();
        s.monstersDefeated = itemQuery.value(2).toInt();
        stats.itemStats.push_back(s);
    }

    QSqlQuery monQuery(db);
    monQuery.prepare(
        "SELECT monster_name, times_used FROM monster_usage "
        "WHERE hero_id = ? ORDER BY monster_name");
    monQuery.addBindValue(heroId);
    monQuery.exec();
    while (monQuery.next()) {
        MonsterStat s;
        s.monsterName = monQuery.value(0).toString().toStdString();
        s.timesUsed = monQuery.value(1).toInt();
        stats.monsterStats.push_back(s);
    }

    return stats;
}