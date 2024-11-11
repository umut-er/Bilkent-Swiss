#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <numeric>

#include "../include/Tournament.h"

Tournament::Tournament() : round(0), tournament_started(false) {

}

Tournament::Tournament(std::string tournament_name, std::string tournament_city, 
                        std::string federation, std::string chief_arbiter, int rounds) 
    : round(0), max_rounds(rounds), tournament_started(false), tournament_name(tournament_name),
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
        p.player_matches.push_back(Match(i+1, p.id, MatchResult::UNMATCHED));
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
    player_list.erase(it);
}

void Tournament::remove_player_idx(int idx){
    player_list.erase(player_list.begin() + idx);
}

void Tournament::deactivate_player(std::string name){
    auto it = std::find_if(player_list.begin(), player_list.end(), [name](Player& player){
        return player.name == name;
    });
    it->active = false;
}

void Tournament::deactivate_player_idx(int idx){
    player_list[idx].active = false;
}

void Tournament::activate_player(std::string name){
    auto it = std::find_if(player_list.begin(), player_list.end(), [name](Player& player){
        return player.name == name;
    });
    it->active = true;
}

void Tournament::activate_player_idx(int idx){
    player_list[idx].active = true;
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
    for(int i = 0; i < (int)player_list.size(); i++){
        player_id_to_idx.insert(std::make_pair(player_list[i].id, i));
    }
}

Tournament Tournament::read_trf_file(const std::string& path){
    // Tokenize the input file
    std::ifstream trf_file(path);
    std::vector<std::string> file_lines;
    std::string file_line;
    while(std::getline(trf_file, file_line)){
        file_lines.push_back(file_line);
    }
    Tournament t;

    // Interpret.
    for(const std::string& line: file_lines) {
        if(line.size() < 3)
            continue;

        std::string data_identification_number = line.substr(0, 3); 
        if(data_identification_number == "XXC") {
            t.first_table_white = (line.substr(4) == "white1");
        }
        else if(data_identification_number == "XXR") {
            t.max_rounds = std::stoi(line.substr(4));
        }
        else if(data_identification_number == "001") {
            int starting_rank = std::stoi(line.substr(4, 4));
            // std::string sex = line.substr(9, 1);
            // std::string fide_title = line.substr(10, 3);
            std::string name = line.substr(14, 32);
            int fide_rating = std::stoi(line.substr(48, 4));
            // std::string federation = line.substr(53, 3);
            // int fide_id = std::stoi(line.substr(57, 11));
            // std::string birth_date = line.substr(69, 10);
            int points = (int)(2 * std::stof(line.substr(80, 4)));
            // int rank = std::stoi(line.substr(85, 4));

            Player p(name, fide_rating);
            p.id = starting_rank;
            p.points = points;

            int end_idx = 98, idx = 0;
            while(end_idx < (int)line.size()){
                idx++;
                int start_idx = end_idx - 7;
                std::string opponent_string = line.substr(start_idx, 4);
                start_idx += 5;
                char color = line.substr(start_idx, 1)[0];
                start_idx += 2;
                char result = line.substr(start_idx, 1)[0];
                start_idx += 3;

                // TODO: create matches
                Match m;
                m.round = idx;
                if(opponent_string == "    " || opponent_string == "0000"){
                    m.white_player_id = starting_rank;
                    m.black_player_id = -1;
                    if(result == 'h' || result == 'H')
                        m.game_result = MatchResult::HALF_POINT_BYE;
                    else if(result == 'f' || result == 'F')
                        m.game_result = MatchResult::FULL_POINT_BYE;
                    else if(result == 'u' || result == 'U')
                        m.game_result = MatchResult::PAIRING_ALLOCATED_BYE;
                    else if(result == 'z' || result == 'Z' || result == ' ')
                        m.game_result = MatchResult::UNMATCHED;
                    p.player_matches.push_back(m);
                    end_idx += 10;
                    continue;
                }
                int opponent = std::stoi(opponent_string);
                bool color_bool = (color == 'w' || color == 'W');
                if(color == 'w' || color == 'W'){
                    m.white_player_id = starting_rank;
                    m.black_player_id = opponent;
                }
                else if(color == 'b' || color == 'B'){
                    m.white_player_id = opponent;
                    m.black_player_id = starting_rank;
                }
                
                // Result 
                if(result == '-')
                    m.game_result = color_bool ? MatchResult::FORFEIT_BLACK_WIN : MatchResult::FORFEIT_WHITE_WIN;
                else if(result == '+')
                    m.game_result = color_bool ? MatchResult::FORFEIT_WHITE_WIN : MatchResult::FORFEIT_BLACK_WIN;
                else if(result == 'w' || result == 'W')
                    m.game_result = color_bool ? MatchResult::UNRATED_WHITE_WIN : MatchResult::UNRATED_BLACK_WIN;
                else if(result == 'd' || result == 'D')
                    m.game_result = MatchResult::UNRATED_DRAW;
                else if(result == 'l' || result == 'L')
                    m.game_result = color_bool ? MatchResult::UNRATED_BLACK_WIN : MatchResult::UNRATED_WHITE_WIN;
                else if(result == '1')
                    m.game_result = color_bool ? MatchResult::REGULAR_WHITE_WIN : MatchResult::REGULAR_BLACK_WIN;
                else if(result == '=')
                    m.game_result = MatchResult::REGULAR_DRAW;
                else if(result == '0')
                    m.game_result = color_bool ? MatchResult::REGULAR_BLACK_WIN : MatchResult::REGULAR_WHITE_WIN;

                p.player_matches.push_back(m);
                end_idx += 10;
            }
            if(t.round != 0)
                t.round = std::min(idx, t.round);
            else
                t.round = idx;

            t.player_list.push_back(p);
        }
        else if(data_identification_number == "012") {
            t.tournament_name = line.substr(4);
        }
        else if(data_identification_number == "022") {
            t.tournament_city = line.substr(4);
        }
        else if(data_identification_number == "032") {
            t.federation = line.substr(4);
        }
        else if(data_identification_number == "042") {
            // date of start
        }
        else if(data_identification_number == "052") {
            // date of end
        }
        else if(data_identification_number == "062") {
            // number of players
        }
        else if(data_identification_number == "072") {
            // number of rated players
        }
        else if(data_identification_number == "082") {
            // number of teams
        }
        else if(data_identification_number == "092") {
            // type of tournament
        }
        else if(data_identification_number == "102") {
            t.chief_arbiter = line.substr(4);
        }
        else if(data_identification_number == "112") {
            // deputy chief arbiter (one line for each arbiter)
        }
        else if(data_identification_number == "122") {
            // allotted times per moves/game
        }
        else if(data_identification_number == "132") {
            // dates of the round  YY/MM/DD
        }
    }

    // TODO: Validate tournament, especially match results. Also, set FORFEIT_BOTHs;
    if(t.round > 0)
        t.start_tournament();
    return t;
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
    output_trf << "XXR " << max_rounds << "\n";

    int idx = 1;
    for(const Player& player : player_list){
        output_trf << "001 ";
        output_trf << std::right << std::setw(4) << idx << " ";
        output_trf << "m";
        output_trf << std::left << std::setw(3) << "   " << " "; // FOR FIDE TITLE
        output_trf << std::left << std::setw(33) << player.name << " ";
        output_trf << std::left << std::setw(4) << player.rating << " ";
        output_trf << std::left << std::setw(3) << "TUR" << " "; // FOR FIDE FEDERATION
        output_trf << std::right << std::setw(11) << "00000000" << " "; // FIDE ID
        output_trf << std::left << std::setw(10) << "2024" << " "; // BIRTH DATE (YYYY/MM/DD)
        output_trf << std::right << std::setw(4) << std::fixed << std::setprecision(1) << ((float)player.points/2.) << " "; // POINTS
        output_trf << std::left << std::setw(4) << " " << "  "; // RANK

        for(const Match& match : player.player_matches){
            std::string opponent_string;
            if(match.match_no_opponent())
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

        if(!player.active)
            output_trf << "     - Z  ";

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
    int number_of_pairs = 0; bool bye_present = false;
    pairing_stream >> number_of_pairs;
    for(int i = 0; i < number_of_pairs; i++){
        int first_player_idx, second_player_idx;
        pairing_stream >> first_player_idx >> second_player_idx;
        if(second_player_idx == 0){ // BYE CONDITION
            int first_player_id = player_list[first_player_idx-1].id;
            pairings.push_back(std::make_pair(first_player_id, -1));
            bye_present = true;
            continue;
        }
        first_player_idx--; second_player_idx--;
        int first_player_id = player_list[first_player_idx].id;
        int second_player_id = player_list[second_player_idx].id;
        pairings.push_back(std::make_pair(first_player_id, second_player_id));
    }

    pairing_results.clear();
    pairing_results.resize(pairings.size(), MatchResult::UNINITIALIZED);
    if(bye_present)
        pairing_results.back() = MatchResult::PAIRING_ALLOCATED_BYE;
}

void Tournament::enter_pairing_result(int idx, MatchResult res){
    pairing_results[idx] = res;
}

void Tournament::generate_ranking(){
    rankings_ids.clear();
    rankings_ids.resize(player_list.size());
    std::iota(rankings_ids.begin(), rankings_ids.end(), 0);

    std::sort(rankings_ids.begin(), rankings_ids.end(), [this](const int& p1, const int& p2){
        if(this->player_list[p1].points != this->player_list[p2].points)
            return this->player_list[p1].points > this->player_list[p2].points;
        return p1 < p2;
    });

    for(int i = 0; i < (int)rankings_ids.size(); i++){
        rankings_ids[i] = player_list[rankings_ids[i]].id;
    }
}
