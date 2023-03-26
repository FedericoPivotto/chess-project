// Pivotto Federico
#include "player.h"

Player::Player()
    : color{0}, type{0} {}

Player::Player(char clr, char typ)
    : color{clr}, type{typ} {}

Player::Player(const Player& p)
    : color{p.color}, type{p.type} {}

char Player::get_color() const {
    return color;
}

char Player::get_type() const {
    return type;
}

int Player::get_passive_streak() const {
    return passive_streak;
}

void Player::incr_passive_streak() {
    passive_streak++;
}

void Player::reset_passive_streak() {
    passive_streak = 0;
}

const std::vector<std::shared_ptr<Piece>>& Player::get_pieces() const {
    return pieces;
}

std::vector<std::shared_ptr<Piece>>& Player::get_pieces() {
    return pieces;
}