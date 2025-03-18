cmake_minimum_required(VERSION 3.10)

project(ChessBot)

set(CMAKE_CXX_STANDARD 23)

find_package(SFML 2.6.1 COMPONENTS graphics window system REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(ChessBot src/main.cpp src/Board.cpp)

target_link_libraries(ChessBot sfml-graphics sfml-window sfml-system)
