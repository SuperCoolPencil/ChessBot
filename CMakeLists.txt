cmake_minimum_required(VERSION 3.10)
project(ChessBot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(ChessBot ${SOURCES})
target_link_libraries(ChessBot sfml-graphics sfml-window sfml-system)
