#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <string>
#include <vector>

#include "Match.h"

/**
 * A basic class for a player in the tournament.
 */
class Player {
private:
    static int CURRENT_ID;

public:
    int id;
    std::string name;
    bool active;
    int rating;

    int points;
    std::vector<Match> player_matches;

    Player(std::string player_name, int player_rating);
};

#endif
