#include "Match.h"

std::map<MatchResult, std::pair<int, int>> result_to_points = {
    {MatchResult::REGULAR_WHITE_WIN, {2, 0}},
    {MatchResult::REGULAR_BLACK_WIN, {0, 2}},
    {MatchResult::REGULAR_DRAW, {1, 1}},

    {MatchResult::FORFEIT_WHITE_WIN, {2, 0}},
    {MatchResult::FORFEIT_BLACK_WIN, {0, 2}},
    {MatchResult::FORFEIT_BOTH, {0, 0}},

    {MatchResult::UNRATED_WHITE_WIN, {2, 0}},
    {MatchResult::UNRATED_BLACK_WIN, {0, 2}},
    {MatchResult::UNRATED_DRAW, {1, 1}},

    {MatchResult::HALF_POINT_BYE, {1, 0}},
    {MatchResult::FULL_POINT_BYE, {2, 0}},
    {MatchResult::PAIRING_ALLOCATED_BYE, {2, 0}},

    {MatchResult::UNMATCHED, {0, 0}},
};

std::map<PlayerResult, char> result_to_rtfchar = {
    { PlayerResult::REGULAR_WIN, '1'},
    { PlayerResult::FORFEIT_WIN, '+'},
    { PlayerResult::UNRATED_WIN, 'W'},

    { PlayerResult::REGULAR_DRAW, '='},
    { PlayerResult::UNRATED_DRAW, 'D'},

    { PlayerResult::REGULAR_LOSS, '0'},
    { PlayerResult::FORFEIT_LOSS, '-'},
    { PlayerResult::UNRATED_LOSS, 'L'},

    { PlayerResult::FULL_POINT_BYE, 'F'},
    { PlayerResult::HALF_POINT_BYE, 'H'},
    { PlayerResult::PAIRING_ALLOCATED_BYE, 'U'},
    { PlayerResult::ABSENT, 'Z'},
    { PlayerResult::UNDETERMINED, 'Z'},

};

Match::Match(int round, int white_id)
    : round(round), white_player_id(white_id), black_player_id(-1), game_result(MatchResult::UNMATCHED){

    }

Match::Match(int round, int white_id, int black_id, MatchResult result)
    : round(round), white_player_id(white_id), black_player_id(black_id), game_result(result){

}

PlayerResult Match::get_player_result(int player_id) const{
    if(player_id != white_player_id && player_id != black_player_id)
        return PlayerResult::UNDETERMINED;

    bool player_is_white = (player_id == white_player_id);

    // REGULAR RESULTS
    if(game_result == MatchResult::REGULAR_WHITE_WIN)
        return player_is_white ? PlayerResult::REGULAR_WIN : PlayerResult::REGULAR_LOSS;
    else if(game_result == MatchResult::REGULAR_BLACK_WIN)
        return player_is_white ? PlayerResult::REGULAR_LOSS : PlayerResult::REGULAR_WIN;
    else if(game_result == MatchResult::REGULAR_DRAW)
        return PlayerResult::REGULAR_DRAW;

    // FORFEIT RESULTS
    else if(game_result == MatchResult::FORFEIT_WHITE_WIN)
        return player_is_white ? PlayerResult::FORFEIT_WIN : PlayerResult::FORFEIT_LOSS;
    else if(game_result == MatchResult::FORFEIT_BLACK_WIN)
        return player_is_white ? PlayerResult::FORFEIT_LOSS : PlayerResult::FORFEIT_WIN;
    else if(game_result == MatchResult::FORFEIT_BOTH)
        return PlayerResult::FORFEIT_LOSS;

    // UNRATED RESULTS
    else if(game_result == MatchResult::UNRATED_WHITE_WIN)
        return player_is_white ? PlayerResult::UNRATED_WIN : PlayerResult::UNRATED_LOSS;
    else if(game_result == MatchResult::UNRATED_BLACK_WIN)
        return player_is_white ? PlayerResult::UNRATED_LOSS : PlayerResult::UNRATED_WIN;
    else if(game_result == MatchResult::UNRATED_DRAW)
        return PlayerResult::UNRATED_DRAW;

    // BYES
    else if(game_result == MatchResult::HALF_POINT_BYE)
        return player_is_white ? PlayerResult::HALF_POINT_BYE : PlayerResult::UNDETERMINED;
    else if(game_result == MatchResult::FULL_POINT_BYE)
        return player_is_white ? PlayerResult::FULL_POINT_BYE : PlayerResult::UNDETERMINED;
    else if(game_result == MatchResult::PAIRING_ALLOCATED_BYE)
        return player_is_white ? PlayerResult::PAIRING_ALLOCATED_BYE : PlayerResult::UNDETERMINED;

    // ABSENT
    else if(game_result == MatchResult::UNMATCHED){
        return PlayerResult::ABSENT;
    }

    return PlayerResult::UNDETERMINED;
}

int Match::get_opponent_id(int player_id) const{
    return white_player_id == player_id ? black_player_id : white_player_id;
}

int Match::get_player_color(int player_id) const{
    if(match_is_bye() || game_result == MatchResult::UNMATCHED)
        return -1;
    return player_id == black_player_id;
}

bool Match::match_is_bye() const{
    return game_result == MatchResult::PAIRING_ALLOCATED_BYE 
            || game_result == MatchResult::FULL_POINT_BYE || game_result == MatchResult::HALF_POINT_BYE;
}
