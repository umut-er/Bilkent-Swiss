#ifndef _MATCH_H_
#define _MATCH_H_

#include <memory>
#include <map>
#include <string>

enum class MatchResult {
    REGULAR_WHITE_WIN,
    REGULAR_BLACK_WIN, 
    REGULAR_DRAW,

    FORFEIT_WHITE_WIN,
    FORFEIT_BLACK_WIN,
    FORFEIT_BOTH,

    UNRATED_WHITE_WIN,
    UNRATED_BLACK_WIN,
    UNRATED_DRAW,

    HALF_POINT_BYE,
    FULL_POINT_BYE,
    PAIRING_ALLOCATED_BYE,

    UNMATCHED,          // For absent players
    UNINITIALIZED
};

enum class PlayerResult {
    REGULAR_WIN,
    FORFEIT_WIN,
    UNRATED_WIN,                // game lasted less than 1 move

    REGULAR_DRAW,
    UNRATED_DRAW,               // game lasted less than 1 move

    REGULAR_LOSS,
    FORFEIT_LOSS,
    UNRATED_LOSS,               // game lasted less than 1 move

    FULL_POINT_BYE,
    HALF_POINT_BYE,
    PAIRING_ALLOCATED_BYE,
    ABSENT,

    UNDETERMINED
};

extern std::map<MatchResult, std::pair<int, int>> result_to_points;
extern std::map<MatchResult, std::string> result_to_string;
extern std::map<PlayerResult, char> result_to_rtfchar;
extern std::map<PlayerResult, int> player_result_to_points;
extern std::map<char, PlayerResult> rtfchar_to_result;

/**
 * Basic Class for Recording Games
 */
class Match{
public:
    int round;
    int white_player_id;
    int black_player_id;
    int white_cur_score;
    int black_cur_score;
    MatchResult game_result;

    Match();
    Match(int round, int white_id, int white_cur_score, MatchResult result);     // Constructor for absentee or bye games.
    Match(int round, int white_id, int black_id, int white_cur_score, int black_cur_score, MatchResult result);

    PlayerResult get_player_result(int player_id) const;
    int get_opponent_id(int player_id) const;
    int get_player_color(int player_id) const; // 0 -> white, 1 -> black, -1 -> bye, unmatched
    bool match_no_opponent() const;
};

#endif
