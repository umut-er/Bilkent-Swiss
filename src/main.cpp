#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL2/SDL.h>
#include <iostream>

#include "Tournament.h"

int initialize(SDL_Renderer*& renderer, SDL_Window*& window, ImGuiIO& io){
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif


    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("Bilkent Swiss", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();
    ImGui::GetStyle().WindowRounding = 0.0f;

    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    return 0;
}

Tournament load_tournament(char tournament_name[], char city[],
                            char federation[], char chief_arbiter[]){
    Tournament t(tournament_name, city, federation, chief_arbiter);
    return t;
}

void show_create_tournament_window(bool* p_open, Tournament& t, bool* tournament_loaded, bool* tournament_started){
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 3, 158));

    static char tournament_name[128] = "";
    static char city[128] = "";
    static char federation[128] = "";
    static char chief_arbiter[128] = "";
    ImGui::Begin("Create Tournament", p_open, ImGuiWindowFlags_NoResize);
    if(ImGui::BeginTable("Tournament Info", 2, ImGuiTableFlags_SizingFixedFit)){
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);

        // TOURNAMENT NAME
        ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
        ImGui::Text("Tournament Name");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
        ImGui::InputText("Tournament Name", tournament_name, 128);
        ImGui::PopItemWidth();

        // TOURNAMENT CITY
        ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
        ImGui::Text("City");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
        ImGui::InputText("City", city, 128);
        ImGui::PopItemWidth();

        // TOURNAMENT FEDERATION
        ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
        ImGui::Text("Federation");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
        ImGui::InputText("Federation", federation, 128);
        ImGui::PopItemWidth();

        // CHIEF ARBITER
        ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
        ImGui::Text("Chief Arbiter");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
        ImGui::InputText("Chief Arbiter", chief_arbiter, 128);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }

    ImGui::Separator();

    if(ImGui::BeginTable("Create Tournament Create/Cancel", 2, ImGuiTableFlags_SizingStretchSame)){
        ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
        if(ImGui::Button("Create", ImVec2(-FLT_MIN, 0))){
            t = load_tournament(tournament_name, city, federation, chief_arbiter);
            *tournament_loaded = true;
            *tournament_started = false;
            *p_open = false;
        }
        ImGui::TableSetColumnIndex(1);
        if(ImGui::Button("Cancel", ImVec2(-FLT_MIN, 0))){
            *p_open = false;
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void show_add_player_window(bool* p_open, Tournament& t){
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), ImGuiCond_Appearing, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 3, 112));

    static char player_name[128];
    static int player_rating;
    ImGui::Begin("Create Tournament", p_open, ImGuiWindowFlags_NoResize);
    if(ImGui::BeginTable("Tournament Info", 2, ImGuiTableFlags_SizingFixedFit)){
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Input", ImGuiTableColumnFlags_WidthStretch);

        // PLAYER NAME
        ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
        ImGui::Text("Player Name");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
        ImGui::InputText("Player Name", player_name, 128);
        ImGui::PopItemWidth();

        // PLAYER RATING
        ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
        ImGui::Text("Player Rating");

        ImGui::TableSetColumnIndex(1);
        ImGui::PushItemWidth(-1);
        ImGui::InputInt("Player Rating", &player_rating);
        ImGui::PopItemWidth();

        ImGui::EndTable();
    }

    ImGui::Separator();

    if(ImGui::BeginTable("Add Player Create/Cancel", 2, ImGuiTableFlags_SizingStretchSame)){
        ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
        if(ImGui::Button("Create", ImVec2(-FLT_MIN, 0))){
            t.add_player(player_name, player_rating);
            *p_open = false;
        }
        ImGui::TableSetColumnIndex(1);
        if(ImGui::Button("Cancel", ImVec2(-FLT_MIN, 0))){
            *p_open = false;
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void show_initial_ranking_listing(Tournament& tournament, bool tournament_loaded, int& player_selected_idx){
    bool hovered = false;
    if (ImGui::BeginTabItem("Initial Rankings")){
        if(tournament_loaded){
            float windowWidth = ImGui::GetWindowSize().x;
            ImVec2 textSize = ImGui::CalcTextSize(tournament.tournament_name.c_str());
            float textX = (windowWidth - textSize.x) * 0.5f;
            if (textX > 0.0f) {
                ImGui::SetCursorPosX(textX);
            }
            ImGui::Text(tournament.tournament_name.c_str());
            if(ImGui::BeginTable("Tournament Info", 3, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)){
                ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Rating", ImGuiTableColumnFlags_WidthFixed);

                // Headers
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("SNo");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("Name");
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("Rtg");
                
                int idx = 1;
                for(Player& player : tournament.player_list){
                    std::string idx_string = std::string(3 - std::to_string(idx).length(), ' ') + std::to_string(idx);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if(ImGui::Selectable(idx_string.c_str(), player_selected_idx == idx-1,
                        ImGuiSelectableFlags_SpanAllColumns)){
                        if(player_selected_idx == idx-1)
                            player_selected_idx = -1;
                        else
                            player_selected_idx = idx-1;
                    }
                    hovered |= ImGui::IsItemHovered();
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text(player.name.c_str());
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%4d", player.rating);
                    idx++;
                }
                ImGui::EndTable();
            }
            if (ImGui::IsMouseClicked(0) && !hovered) {
                player_selected_idx = -1;  // Deselect if clicking outside the table
            }

        }
        ImGui::EndTabItem();
    }
}

void show_pairing_listing(Tournament& tournament, int& pairing_selected_idx){
    bool hovered = false;
    if (ImGui::BeginTabItem("Pairings")){
        if(tournament.round > 0){
            float windowWidth = ImGui::GetWindowSize().x;
            ImVec2 textSize = ImGui::CalcTextSize(tournament.tournament_name.c_str());
            float textX = (windowWidth - textSize.x) * 0.5f;
            if (textX > 0.0f) {
                ImGui::SetCursorPosX(textX);
            }
            ImGui::Text(tournament.tournament_name.c_str());
            if(ImGui::BeginTable("Tournament Pairings", 8, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)){
                ImGui::TableSetupColumn("Board1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("WhiteName1", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("WhiteRating1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("WhitePoint1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Result1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("BlackPoint1", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("BlackName1", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("BlackRating1", ImGuiTableColumnFlags_WidthFixed);

                // Table Headers
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Bo.");
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("White");
                ImGui::TableSetColumnIndex(2);
                ImGui::Text("Rtg");
                ImGui::TableSetColumnIndex(3);
                ImGui::Text("Pts.");
                ImGui::TableSetColumnIndex(4);
                ImGui::Text("Result");
                ImGui::TableSetColumnIndex(5);
                ImGui::Text("Pts.");
                ImGui::TableSetColumnIndex(6);
                ImGui::Text("Black");
                ImGui::TableSetColumnIndex(7);
                ImGui::Text("Rtg");
                
                int idx = 1;
                for(std::pair<int, int> pair : tournament.pairings){
                    int first_player_idx = tournament.player_id_to_idx.at(pair.first);
                    int second_player_idx = pair.second < 0 ? -1 : tournament.player_id_to_idx.at(pair.second);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%3d", idx);
                    ImGui::TableSetColumnIndex(1);
                    if(ImGui::Selectable(tournament.player_list[first_player_idx].name.c_str(), pairing_selected_idx == idx-1,
                                        ImGuiSelectableFlags_SpanAllColumns)){
                        if(pairing_selected_idx == idx-1)
                            pairing_selected_idx = -1;
                        else
                            pairing_selected_idx = idx-1;
                    }
                    hovered |= ImGui::IsItemHovered();
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%4d", tournament.player_list[first_player_idx].rating);
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%2.1f", tournament.player_list[first_player_idx].points / 2.);
                    ImGui::TableSetColumnIndex(4);
                    ImGui::Text(" ");
                    ImGui::TableSetColumnIndex(5);
                    ImGui::Text("%2.1f", tournament.player_list[second_player_idx].points / 2.);
                    ImGui::TableSetColumnIndex(6);
                    std::string black_name_field = "";
                    if(second_player_idx < 0)
                        black_name_field = "Bye";
                    else{
                        black_name_field = tournament.player_list[second_player_idx].name;
                    }
                    ImGui::Text(black_name_field.c_str());
                    ImGui::TableSetColumnIndex(7);
                    if(second_player_idx < 0)
                        ImGui::Text("    ");
                    else
                        ImGui::Text("%4d", tournament.player_list[second_player_idx].rating);
                    idx++;
                }

                ImGui::EndTable();
            }
            if (ImGui::IsMouseClicked(0) && !hovered) {
                pairing_selected_idx = -1;  // Deselect if clicking outside the table
            }
        }
        ImGui::EndTabItem();
    }
}

void show_ranking_listing(Tournament& tournament){
    if (ImGui::BeginTabItem("Rankings")){
        ImGui::Text("This page displays current rankings after the most recently finished round.");
        ImGui::EndTabItem();
    }
}

int main(){
    SDL_Renderer* renderer; SDL_Window* window; ImGuiIO io;
    int err_code = initialize(renderer, window, io);
    if(err_code != 0)
        return -1;

    // Our state variables
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    Tournament tournament;
    bool show_create_tournament = false;
    bool show_add_player = false;
    bool tournament_loaded = false;
    bool tournament_started = false;
    static int player_selected_idx = -1;
    static int pairing_selected_idx = -1;

    // Main loop
    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        {
            io = ImGui::GetIO();
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
            ImGui::Begin("Bilkent Swiss", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse
                | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize); // ImGuiWindowFlags_NoDecoration
            if (ImGui::BeginMenuBar()){ 
                if (ImGui::BeginMenu("File")){
                    ImGui::MenuItem("Load Tournament", "Ctrl-O");       // Load TRF file.
                    ImGui::MenuItem("Save Tournament", "Ctrl-S");       // Save as TRF.
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::BeginChild("Tournament", ImVec2(ImGui::GetContentRegionAvail().x * 0.7f, ImGui::GetContentRegionAvail().y),
                    ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
            if(ImGui::BeginTabBar("Listings")){
                show_initial_ranking_listing(tournament, tournament_loaded, player_selected_idx);
                show_pairing_listing(tournament, pairing_selected_idx);
                show_ranking_listing(tournament);
                ImGui::EndTabBar();
            }
            ImGui::EndChild();  

            ImGui::SameLine();

            ImGui::BeginChild("Manage", ImVec2(0, 0), ImGuiChildFlags_Borders);
            if(ImGui::BeginTabBar("Tournament Management")){
                if (ImGui::BeginTabItem("Edit Tournament")){

                    if(ImGui::Button("Create New Tournament", ImVec2(-FLT_MIN, 30))){
                        show_create_tournament = true;
                    }

                    if(!tournament_loaded)
                        ImGui::BeginDisabled();
                    if(ImGui::Button("Add Player", ImVec2(-FLT_MIN, 30)))
                        show_add_player = true;

                    if(player_selected_idx == -1)
                        ImGui::BeginDisabled();
                    if(ImGui::Button("Edit Player", ImVec2(-FLT_MIN, 30))){

                    }
                    if(tournament_started)
                        ImGui::BeginDisabled();
                    if(ImGui::Button("Remove Player", ImVec2(-FLT_MIN, 30))){

                    }
                    if(tournament_started)
                        ImGui::EndDisabled();
                    if(player_selected_idx == -1)
                        ImGui::EndDisabled();

                    [&tournament_started, &tournament](bool disable_this_button){
                        if(disable_this_button)
                            ImGui::BeginDisabled();
                        if(ImGui::Button("Start Tournament", ImVec2(-FLT_MIN, 30))){
                            tournament.start_tournament();
                            tournament_started = true;
                        }
                        if(disable_this_button)
                            ImGui::EndDisabled();
                    }(tournament_started);

                    if(!tournament_loaded)
                        ImGui::EndDisabled();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Manage Pairings")){
                    if(!tournament_started)
                        ImGui::BeginDisabled();
                    if(ImGui::Button("Pair Next Round", ImVec2(-FLT_MIN, 30))){
                        tournament.create_pairing();
                    }
                    ImGui::Button("Add Player to Pairings", ImVec2(-FLT_MIN, 30));
                    ImGui::Button("Remove Player from Pairings", ImVec2(-FLT_MIN, 30));

                    ImGui::Separator();

                    if(pairing_selected_idx == -1)
                        ImGui::BeginDisabled();
                    if(ImGui::BeginTable("Tournament Info", 3, ImGuiTableFlags_SizingFixedFit)){
                        ImGui::TableSetupColumn("ROW1", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("ROW2", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("ROW3", ImGuiTableColumnFlags_WidthStretch);

                        // Regular
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Button("1-0", ImVec2(-FLT_MIN, 30));
                        ImGui::TableNextColumn();
                        ImGui::Button("1/2-1/2", ImVec2(-FLT_MIN, 30));
                        ImGui::TableNextColumn();
                        ImGui::Button("0-1", ImVec2(-FLT_MIN, 30));

                        // Forfeit
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Button("+/-", ImVec2(-FLT_MIN, 30));
                        ImGui::TableNextColumn();
                        ImGui::Button("=/=", ImVec2(-FLT_MIN, 30));
                        ImGui::TableNextColumn();
                        ImGui::Button("-/-", ImVec2(-FLT_MIN, 30));

                        // Unrated
                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::Button("W/L", ImVec2(-FLT_MIN, 30));
                        ImGui::TableNextColumn();
                        ImGui::Button("D/D", ImVec2(-FLT_MIN, 30));
                        ImGui::TableNextColumn();
                        ImGui::Button("L/W", ImVec2(-FLT_MIN, 30));

                        ImGui::EndTable();
                    }
                    if(pairing_selected_idx == -1)
                        ImGui::EndDisabled();
                    if(!tournament_started)
                        ImGui::EndDisabled();

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndChild();  
            ImGui::End();
        }

        if(show_create_tournament)
            show_create_tournament_window(&show_create_tournament, tournament, &tournament_loaded, &tournament_started);

        if(show_add_player)
            show_add_player_window(&show_add_player, tournament);

        ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
