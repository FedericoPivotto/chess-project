# Genilotti Fabrizio
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

project(chess)

include_directories(scacchiera/include replay/include )

# modulo scacchiera 

add_library(pair
scacchiera/include/pair/pair.h
scacchiera/include/pair/pair.cpp
)

add_library(chessboard
scacchiera/include/chessboard/chessboard.h
scacchiera/include/chessboard/chessboard.cpp
scacchiera/include/chessboard/movement.cpp
)

add_library(pieces
scacchiera/include/piece/piece.h
scacchiera/include/piece/piece.cpp
)

add_library(player
scacchiera/include/player/player.h
scacchiera/include/player/player.cpp
)

add_library(menu
scacchiera/include/menu/menu.h
scacchiera/include/menu/menu.cpp
)

add_executable(scacchiera
scacchiera/src/main.cpp
)

target_link_libraries(scacchiera
pair
chessboard
pieces
player
menu
)

# modulo replay 

add_library(replay_lib
replay/include/replay/replay.h
replay/include/replay/replay.cpp
)

add_executable(replay
replay/src/main.cpp
)

target_link_libraries(replay
replay_lib
)

