cmake_minimum_required(VERSION 3.14)
project(MonsterAdventure VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    Main.cpp
    Monster.cpp
    MonsterRegistry.cpp
    Character.cpp
    Battle.cpp
    Game.cpp
    InputHelper.cpp
)

add_executable(monster_game ${SOURCES})

# Include headers from the same directory
target_include_directories(monster_game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Enable warnings (GCC/Clang only, for Ubuntu)
target_compile_options(monster_game PRIVATE -Wall -Wextra)
