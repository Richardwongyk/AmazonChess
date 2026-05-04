#ifndef GAME_H
#define GAME_H

#include "constants.h"
#include "game_board.h"
#include "SaveManager.h"
#include "renderer.h"
#include <string>
#include <utility>
#include <vector>
#include <memory>

class IPlatform;

class Game {
public:
    explicit Game(IPlatform* platform);
    ~Game() = default;

    bool initialize();
    void run();
    void reset_game();
    void deleteCurrentGame();
    void set_game_mode(bool black_human, bool white_human);
    void set_game_state(GameConstants::GameState state);
    void set_difficulty(int difficulty) { difficulty_ = difficulty; }
    int  get_difficulty() const { return difficulty_; }
    friend class SaveManager;

private:
    IPlatform* platform_;

    std::string current_game_id_;
    int selected_mode_;
    int difficulty_;
    std::unique_ptr<GameBoard> board_;
    std::unique_ptr<Renderer> renderer_;

    GameConstants::GameState current_state_;
    bool black_is_human_;
    bool white_is_human_;

    int move_phase_;
    Move chess_move_;
    Move arrow_move_;

    std::vector<std::pair<int, int>> highlight_moves_;
    std::vector<std::pair<int, int>> highlight_arrows_;
    std::vector<SaveRecord> load_menu_records_;

    void generateGameId();
    void start_game(bool is_black_human, bool is_white_human);
    void handle_load_input();
    void enter_load_menu();
    void handle_menu_input();
    void handle_mode_select_input();
    void handle_difficulty_select_input();
    void handle_game_input();
    void handle_game_over_input();
    void handle_mouse_click();
    void handle_piece_selection(int row, int col);
    void handle_piece_movement(int row, int col);
    void handle_arrow_placement(int row, int col);
    void execute_ai_move();
    void update_game_display();
    void update_highlights();
    void clear_highlights();
};

#endif
