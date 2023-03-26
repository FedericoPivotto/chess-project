// Molinaroli Davide
#ifndef PIECE_H
#define PIECE_H

#include <algorithm>
#include <cctype>
#include <map>
#include <vector>

#include "pair/pair.h"

class Piece {
   public:
    Piece(char identifier, Pair pos, bool a = true, int count = 0);

    static const int DIRECTION_MAX_LENGTH = 7;

    // member functions
    char get_id() const;
    void set_id(char identifier);  // utile per la promozione

    Pair get_pos() const;
    void set_pos(Pair pos);

    const std::vector<Pair>& get_piece_directions() const;
    std::vector<Pair> get_piece_directions();

    bool is_alive() const;
    void set_alive(bool a);
    int get_counter() const;
    void incr_counter();

    Pair dir_to_dest(Pair dest) const;

   private:
    char id;
    Pair position;
    bool alive;
    int counter;
    static const std::map<char, std::vector<Pair>> directions;
};

// operators
bool operator==(const Piece& p1, const Piece& p2);  // controllo sull'id

#endif  // PIECE_H