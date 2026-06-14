cmake_minimum_required(VERSION 3.14)
project(MonsterAdventure VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt5 SQL module (uses the QSQLITE driver from libqt5sql5-sqlite)
set(CMAKE_AUTOMOC ON)
find_package(Qt5 COMPONENTS Sql REQUIRED)

set(SOURCES
    Main.cpp
    Monster.cpp
    MonsterRegistry.cpp
    Character.cpp
    Battle.cpp
    Game.cpp
    InputHelper.cpp
    Item.cpp
    Cave.cpp
    Database.cpp
)

add_executable(monster_game ${SOURCES})

target_include_directories(monster_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(monster_game PRIVATE Qt5::Sql)

target_compile_options(monster_game PRIVATE -Wall -Wextra)
