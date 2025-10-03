#include "Match.h"
#include <iostream>

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
    {MatchResult::UNINITIALIZED, {0, 0}}
};

std::map<MatchResult, std::string> result_to_string = {
    {MatchResult::REGULAR_WHITE_WIN, "1 - 0"},
    {MatchResult::REGULAR_BLACK_WIN, "0 - 1"},
    {MatchResult::REGULAR_DRAW, "1/2 - 1/2"},

    {MatchResult::FORFEIT_WHITE_WIN, "+ / -"},
    {MatchResult::FORFEIT_BLACK_WIN, "- / +"},
    {MatchResult::FORFEIT_BOTH, "- / -"},

    {MatchResult::UNRATED_WHITE_WIN, "W / L"},
    {MatchResult::UNRATED_BLACK_WIN, "L / W"},
    {MatchResult::UNRATED_DRAW, "D / D"},

    {MatchResult::HALF_POINT_BYE, "1 / 2"},
    {MatchResult::FULL_POINT_BYE, " 1 "},
    {MatchResult::PAIRING_ALLOCATED_BYE, " 1 "},

    {MatchResult::UNMATCHED, "  "},
    {MatchResult::UNINITIALIZED, "  "}
};

std::map<PlayerResult, int> player_result_to_points = {
    { PlayerResult::REGULAR_WIN, 2},
    { PlayerResult::FORFEIT_WIN, 2},
    { PlayerResult::UNRATED_WIN, 2},

    { PlayerResult::REGULAR_DRAW, 1},
    { PlayerResult::UNRATED_DRAW, 1},

    { PlayerResult::REGULAR_LOSS, 0},
    { PlayerResult::FORFEIT_LOSS, 0},
    { PlayerResult::UNRATED_LOSS, 0},

    { PlayerResult::FULL_POINT_BYE, 2},
    { PlayerResult::HALF_POINT_BYE, 1},
    { PlayerResult::PAIRING_ALLOCATED_BYE, 2},
    { PlayerResult::ABSENT, 0},
    { PlayerResult::UNDETERMINED, 0},
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

std::map<char, PlayerResult> rtfchar_to_result = {
    { '1', PlayerResult::REGULAR_WIN},
    { '+', PlayerResult::FORFEIT_WIN},
    { 'W', PlayerResult::UNRATED_WIN},

    { '=', PlayerResult::REGULAR_DRAW},
    { 'D', PlayerResult::UNRATED_DRAW},

    { '0', PlayerResult::REGULAR_LOSS},
    { '-', PlayerResult::FORFEIT_LOSS},
    { 'L', PlayerResult::UNRATED_LOSS},

    { 'F', PlayerResult::FULL_POINT_BYE},
    { 'H', PlayerResult::HALF_POINT_BYE},
    { 'U', PlayerResult::PAIRING_ALLOCATED_BYE},
    { 'Z', PlayerResult::ABSENT}

};

Match::Match(){

}

Match::Match(int round, int white_id, int white_cur_score, MatchResult result)
    : round(round), white_player_id(white_id), black_player_id(-1), 
    white_cur_score(white_cur_score), black_cur_score(0), game_result(result){

}

Match::Match(int round, int white_id, int black_id, int white_cur_score, int black_cur_score, MatchResult result)
    : round(round), white_player_id(white_id), black_player_id(black_id),
    white_cur_score(white_cur_score), black_cur_score(black_cur_score), game_result(result){

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
    if(match_no_opponent())
        return -1;
    return player_id == black_player_id;
}

bool Match::match_no_opponent() const{
    return game_result == MatchResult::PAIRING_ALLOCATED_BYE 
            || game_result == MatchResult::FULL_POINT_BYE || game_result == MatchResult::HALF_POINT_BYE
            || game_result == MatchResult::UNMATCHED;
}
