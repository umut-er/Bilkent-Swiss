#ifndef _TOURNAMENT_H_
#define _TOURNAMENT_H_

#include <vector>
#include <memory>
#include <string>
#include <map>

#include "Player.h"

/**
 * A wrapper to handle a full blown swiss tournament.
 * Uses the bbpPairing pairing engine. 
 */
class Tournament{
private:
    void create_initial_ordering();

public:
    int round;
    bool tournament_started;
    std::vector<Player> player_list;
    std::vector<std::pair<int, int>> pairings; // vector of player ids 
    std::map<int, int> player_id_to_idx;

    std::string tournament_name;
    std::string tournament_city;
    std::string federation;
    std::string chief_arbiter;

    Tournament();
    Tournament(std::string tournament_name, std::string tournament_city, 
            std::string federation, std::string chief_arbiter);

    // Utility methods for manipulating players & player information.
    void add_player(std::string name, int rating);
    void remove_player(std::string name);
    void remove_player_idx(int idx);
    void activate_player(std::string name);
    void change_player_rating(std::string name, int new_rating);
    void change_player_name(std::string old_name, std::string new_name);
    void change_player_rating_idx(int idx, int new_rating);
    void change_player_name_idx(int idx, std::string new_name);

    // Initializes the tournament with the given player list.
    void start_tournament();
    
    // Helper method to print out the tournament.
    void print_player_list();
    // Creates the TRF (tournament report file).
    void create_trf_file();
    // Create Pairing for Current Round
    void create_pairing();
    void print_pairing();
    // Read Results for Current Round
    void get_pairing_results();
};

#endif
