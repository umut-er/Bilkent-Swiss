#include "../include/Player.h"

int Player::CURRENT_ID = 1;

Player::Player(std::string player_name, int player_rating)
    : name(player_name), active(true), rating(player_rating), points(0),
    bh_c1(0.0), sb(0.0), aob(0.0) {

    id = CURRENT_ID;
    CURRENT_ID++;
}
