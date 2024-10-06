#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "../include/Tournament.h"

Tournament::Tournament() : round(0), tournament_started(false) {

}

Tournament::Tournament(std::string tournament_name, std::string tournament_city, 
                        std::string federation, std::string chief_arbiter) 
    : round(0), tournament_started(false), tournament_name(tournament_name),
    tournament_city(tournament_city), federation(federation), chief_arbiter(chief_arbiter) {

}

void Tournament::add_player(std::string name, int rating){
    auto it = std::find_if(player_list.begin(), player_list.end(), [name](Player& player){
        return player.name == name;
    });
    if(it != player_list.end())
        return;
    Player p(name, rating);
    for(int i = 0; i < round; i++){
        p.player_matches.push_back(Match(i+1, p.id));
    }
    player_list.push_back(p);

    if(tournament_started){
        create_initial_ordering();
    }
}

void Tournament::remove_player(std::string name){
    auto it = std::find_if(player_list.begin(), player_list.end(), [name](Player& player){
        return player.name == name;
    });
    it->active = false;
}

void Tournament::remove_player_idx(int idx){
    player_list[idx].active = false;
}

void Tournament::activate_player(std::string name){
    auto it = std::find_if(player_list.begin(), player_list.end(), [name](Player& player){
        return player.name == name;
    });
    it->active = true;
}

void Tournament::print_player_list(){
    std::cout << "   " << std::left << std::setw(30) << "PLAYER NAME" << "RATING" << "\n";

    int idx = 1;
    for(Player& p : player_list){
        if(!p.active)
            continue;
        std::cout << std::left << std::setw(3) << idx 
            << std::left << std::setw(30) << p.name << p.rating << "\n";
        idx++;
    }
}

void Tournament::change_player_rating(std::string name, int new_rating){
    auto it = std::find_if(player_list.begin(), player_list.end(), [name](Player& player){
        return player.name == name;
    });
    it->rating = new_rating;
    if(tournament_started)
        create_initial_ordering();
}

void Tournament::change_player_rating_idx(int idx, int new_rating){
    player_list[idx].rating = new_rating;
    if(tournament_started)
        create_initial_ordering();
}

void Tournament::change_player_name(std::string name, std::string new_name){
    auto it = std::find_if(player_list.begin(), player_list.end(), [name](Player& player){
        return player.name == name;
    });
    it->name = new_name;
}

void Tournament::change_player_name_idx(int idx, std::string new_name){
    player_list[idx].name = new_name;
}

// TODO: Add Titles
void Tournament::create_initial_ordering(){
    std::sort(player_list.begin(), player_list.end(), 
        [](Player& p1, Player& p2){
            if(p1.rating != p2.rating)
                return p1.rating > p2.rating;
            return p1.name < p2.name;
        }
    );

    player_id_to_idx.clear();
    for(int i = 0; i < player_list.size(); i++){
        player_id_to_idx.insert(std::make_pair(player_list[i].id, i));
    }
}

void Tournament::start_tournament(){
    if(tournament_started)
        return;
    tournament_started = true;
    create_initial_ordering();
}

void Tournament::create_trf_file(){
    std::ofstream output_trf("out.trf");

    output_trf << "XXC white1\n";
    output_trf << "XXR 9\n";

    int idx = 1;
    for(const Player& player : player_list){
        output_trf << "001 ";
        output_trf << std::right << std::setw(4) << idx << " ";
        output_trf << "m ";
        output_trf << std::left << std::setw(3) << "   " << " "; // FOR FIDE TITLE
        output_trf << std::left << std::setw(32) << player.name << " ";
        output_trf << std::left << std::setw(4) << player.rating << " ";
        output_trf << std::left << std::setw(3) << "TUR" << " "; // FOR FIDE FEDERATION
        output_trf << std::right << std::setw(11) << "00000000" << " "; // FIDE ID
        output_trf << std::left << std::setw(10) << "2024" << " "; // BIRTH DATE (YYYY/MM/DD)
        output_trf << std::right << std::setw(4) << std::fixed << std::setprecision(1) << ((float)player.points/2.) << " "; // POINTS
        output_trf << std::left << std::setw(4) << " " << "  "; // RANK

        for(const Match& match : player.player_matches){
            std::string opponent_string;
            if(match.match_is_bye())
                opponent_string = "    ";
            else{
                int opponent_id = match.get_opponent_id(player.id);
                int opponent_idx = player_id_to_idx[opponent_id];
                opponent_string = std::to_string(++opponent_idx);
            }
            output_trf << std::right << std::setw(4) << opponent_string << " ";

            std::string color_string;
            int color = match.get_player_color(player.id);
            if(color == -1) color_string = "-";
            else if(color == 0) color_string = "w";
            else color_string = "b";
            output_trf << color_string << " ";

            std::string result_string;
            PlayerResult player_result = match.get_player_result(player.id);
            result_string = result_to_rtfchar.at(player_result);
            output_trf << result_string << "  ";
        }

        idx++;
        output_trf << "\n";
    }
}

void Tournament::create_pairing(){
    round++;
    create_trf_file();
    std::string command("./bin/bbpPairings.exe --dutch out.trf -p round.txt");
    int result = system(command.c_str());
    if(result != 0){
       // Pairing not made
    }
    
    pairings.clear();
    std::ifstream pairing_stream("round.txt");
    int number_of_pairs = 0;
    pairing_stream >> number_of_pairs;
    for(int i = 0; i < number_of_pairs; i++){
        int first_player_idx, second_player_idx;
        pairing_stream >> first_player_idx >> second_player_idx;
        if(second_player_idx == 0){ // BYE CONDITION
            int first_player_id = player_list[first_player_idx-1].id;
            pairings.push_back(std::make_pair(first_player_id, -1));
            continue;
        }
        first_player_idx--; second_player_idx--;
        int first_player_id = player_list[first_player_idx].id;
        int second_player_id = player_list[second_player_idx].id;
        pairings.push_back(std::make_pair(first_player_id, second_player_id));
    }
}

void Tournament::print_pairing(){
    for(int i = 0; i < pairings.size(); i++){
        const std::pair<int, int>& pair = pairings[i];
        int first_player_idx = player_id_to_idx.at(pair.first);
        int second_player_idx = (pair.second < 0) ? -1 : player_id_to_idx.at(pair.second);
        std::cout << std::left << std::setw(3) << (i+1) << " ";
        std::cout << std::left << std::setw(33) << player_list[first_player_idx].name << "   |   ";
        std::cout << std::right << std::setw(32) << (second_player_idx == -1 ? "Bye" : player_list[second_player_idx].name) << "\n";
    }
}

// TODO: add match objects to unmatched players.
void Tournament::get_pairing_results(){
    MatchResult result;
    std::string pair_result;
    for(int i = 0; i < pairings.size(); i++){
        if(pairings[i].second != -1){
            std::cout << (i+1) << ": ";
            std::cin >> pair_result;
        }
        if(pairings[i].second == -1)
            result = MatchResult::PAIRING_ALLOCATED_BYE;
        else if(pair_result == "1-0")
            result = MatchResult::REGULAR_WHITE_WIN;
        else if(pair_result == "0-1")
            result = MatchResult::REGULAR_BLACK_WIN;
        else if(pair_result == "1/2")
            result = MatchResult::REGULAR_DRAW;

        Match match(round, pairings[i].first, pairings[i].second, result);
        int first_player_idx = player_id_to_idx[pairings[i].first];
        if(match.match_is_bye()){
            player_list[first_player_idx].points += result_to_points.at(result).first;
            player_list[first_player_idx].player_matches.push_back(match);
            continue;
        }
        int second_player_idx = player_id_to_idx[pairings[i].second];

        // Add Points
        player_list[first_player_idx].points += result_to_points.at(result).first;
        player_list[second_player_idx].points += result_to_points.at(result).second;
        player_list[first_player_idx].player_matches.push_back(match);
        player_list[second_player_idx].player_matches.push_back(match);
    }
}
