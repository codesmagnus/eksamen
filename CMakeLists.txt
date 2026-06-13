cmake_minimum_required(VERSION 3.14)
project(MonsterAdventure VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
)

add_executable(monster_game ${SOURCES})

target_include_directories(monster_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(monster_game PRIVATE -Wall -Wextra)
