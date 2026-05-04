#ifndef RENDERER_H
#define RENDERER_H

#include "constants.h"
#include "SaveManager.h"
#include <string>
#include <vector>
#include <utility>

class IPlatform;
class GameBoard;

class Renderer {
public:
    Renderer(IPlatform* platform,
             int window_width = GameConstants::WINDOW_WIDTH,
             int window_height = GameConstants::WINDOW_HEIGHT);
    ~Renderer() {}

    bool initialize();
    void clear_screen();

    void draw_board(const GameBoard& board,
                    const std::vector<std::pair<int, int>>& highlight_moves = {},
                    const std::vector<std::pair<int, int>>& highlight_arrows = {});

    void draw_game_info(const std::string& current_player,
                        const std::string& game_status,
                        GameConstants::GameState current_state,
                        bool is_game_over);
    void draw_menu();
    void draw_load_menu(const std::vector<SaveRecord>& records);
    void draw_mode_select();
    void draw_difficulty_select();
    void draw_game_over(const std::string& winner);

    void board_to_screen(int row, int col, int& screen_x, int& screen_y);
    bool screen_to_board(int screen_x, int screen_y, int& row, int& col);
    bool is_click_on_board(int screen_x, int screen_y);

private:
    IPlatform* platform_;
    int window_width_;
    int window_height_;
    int board_x_;
    int board_y_;

    void draw_piece(int row, int col, GameConstants::CellState state);
    void draw_selection(int row, int col);
    void draw_board_border();
    void draw_cell_background(int row, int col,
                              bool is_highlighted = false, bool is_move_highlight = true);
};

#endif
