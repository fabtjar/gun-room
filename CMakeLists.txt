cmake_minimum_required(VERSION 3.20)
project(gun_room C)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib)

set(CMAKE_C_STANDARD 11)

add_executable(gun_room src/main.c src/world.h src/player.c src/player.h src/math_utils.c src/math_utils.h src/baddie.c src/baddie.h src/bullet.c src/bullet.h src/coin.c src/coin.h src/boom.c src/boom.h src/dead_baddie.c src/dead_baddie.h)

target_link_libraries(gun_room raylib)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/content DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
