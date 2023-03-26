// Molinaroli Davide
#ifndef MENU_H
#define MENU_H

#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "chessboard/chessboard.h"
#include "pair/pair.h"
#include "piece/piece.h"
#include "player/player.h"

class Menu {
   public:
    Menu();

    // member functions
    std::vector<Player> get_players(int argc, char** argv) const;
    std::vector<Pair> get_move(Player& player) const;
    char get_promotion(const Player& p) const;

    void write_log_line(const std::vector<Pair> move, int status, char prom_id);
    void close_log();
    void print_status(int turn, int status, bool white_moves) const;
    void print_move(const std::vector<Pair>& move) const;

    // exceptions
    class InvalidArgumentException {};
    class FileErrorException {};
    class NotComputerException {};

   private:
    // data members
    const std::string LOG_FILE_NAME = "log.txt";
    std::ofstream log;

    std::vector<Pair> get_human_move() const;
    std::vector<Pair> get_random_move(Player& player) const;

    char get_human_promotion() const;
    char get_random_promotion() const;
};

// helper functions
int get_random_int(int min, int max);
bool is_line_valid(std::string line);
std::vector<Pair> string_to_move(std::string line);
std::string move_to_string(const std::vector<Pair>& move);
std::string move_to_string(Pair src, Pair dest);

#endif  // MENU_H