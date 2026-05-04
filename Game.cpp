#include "game.h"
#include "AI_Kernel.h"
#include "SaveManager.h"
#include "platform/IPlatform.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

Game::Game(IPlatform* platform)
    : platform_(platform)
    , board_(std::make_unique<GameBoard>())
    , renderer_(std::make_unique<Renderer>(platform))
    , current_state_(GameConstants::GAME_MENU)
    , black_is_human_(true)
    , white_is_human_(false)
    , move_phase_(0)
    , current_game_id_("")
    , selected_mode_(-1)
    , difficulty_(GameConstants::DIFFICULTY_MEDIUM)
{
    srand(static_cast<unsigned>(time(nullptr)));
    platform_->openMusic("music.wav");
}

bool Game::initialize() {
    if (!renderer_->initialize()) {
        std::cerr << "渲染器初始化失败！" << std::endl;
        return false;
    }
    board_->initialize();
    set_game_state(GameConstants::GAME_MENU);
    return true;
}

void Game::generateGameId() {
    std::ostringstream oss;
    oss << std::time(nullptr) << "_"
        << (black_is_human_ ? "H" : "A")
        << (white_is_human_ ? "H" : "A");
    current_game_id_ = oss.str();
}

void Game::run() {
    while (platform_->isRunning() && !quit_requested_) {
        switch (current_state_) {
        case GameConstants::GAME_MENU:
            handle_menu_input();
            break;
        case GameConstants::GAME_MODE_SELECT:
            handle_mode_select_input();
            break;
        case GameConstants::GAME_DIFFICULTY_SELECT:
            handle_difficulty_select_input();
            break;
        case GameConstants::GAME_PLAYING:
            handle_game_input();
            break;
        case GameConstants::GAME_LOAD_MENU:
            handle_load_input();
            break;
        case GameConstants::GAME_OVER:
            handle_game_over_input();
            break;
        }

        if (!platform_->isRunning()) break;
        platform_->delayFPS(current_state_ == GameConstants::GAME_PLAYING ? 150 : 60);
    }

    platform_->closeMusic();
    platform_->closeWindow();
}

void Game::reset_game() {
    board_->initialize();
    move_phase_ = 0;
    chess_move_ = Move();
    arrow_move_ = Move();
    generateGameId();
    set_game_state(GameConstants::GAME_PLAYING);
}

void Game::start_game(bool is_black_human, bool is_white_human) {
    set_game_mode(is_black_human, is_white_human);
    reset_game();
}

void Game::deleteCurrentGame() {
    auto records = SaveManager::loadAllRecords();
    int index_to_delete = -1;
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].game_id == current_game_id_) {
            index_to_delete = static_cast<int>(i);
            break;
        }
    }
    if (index_to_delete != -1) {
        SaveManager::deleteRecord(index_to_delete + 1);
    }
}

void Game::set_game_mode(bool black_human, bool white_human) {
    black_is_human_ = black_human;
    white_is_human_ = white_human;
}

void Game::set_game_state(GameConstants::GameState state) {
    current_state_ = state;

    switch (state) {
    case GameConstants::GAME_MENU:
        platform_->stopMusic();
        renderer_->draw_menu();
        break;
    case GameConstants::GAME_MODE_SELECT:
        platform_->stopMusic();
        selected_mode_ = -1;
        renderer_->draw_mode_select();
        break;
    case GameConstants::GAME_DIFFICULTY_SELECT:
        platform_->stopMusic();
        renderer_->draw_difficulty_select();
        break;
    case GameConstants::GAME_PLAYING:
        update_game_display();
        platform_->stopMusic();
        platform_->playMusic(true);
        break;
    case GameConstants::GAME_OVER: {
        std::string winner = board_->is_winner_black() ? "黑方" : "白方";
        renderer_->draw_game_over(winner);
        platform_->stopMusic();
        break;
    }
    case GameConstants::GAME_LOAD_MENU:
        platform_->stopMusic();
        break;
    }
}

void Game::handle_menu_input() {
    if (platform_->isKeyDown(KEY_A)) {
        set_game_state(GameConstants::GAME_MODE_SELECT);
    } else if (platform_->isKeyDown(KEY_B)) {
        enter_load_menu();
    } else if (platform_->isKeyDown(KEY_C) || platform_->isKeyDown(KEY_ESC)) {
        quit_requested_ = true;
    }
}

void Game::handle_mode_select_input() {
    if (platform_->isKeyDown(KEY_1)) {
        start_game(true, true);
    } else if (platform_->isKeyDown(KEY_2)) {
        selected_mode_ = 2;
        set_game_state(GameConstants::GAME_DIFFICULTY_SELECT);
    } else if (platform_->isKeyDown(KEY_3)) {
        selected_mode_ = 3;
        set_game_state(GameConstants::GAME_DIFFICULTY_SELECT);
    } else if (platform_->isKeyDown(KEY_0)) {
        set_game_state(GameConstants::GAME_MENU);
    }
}

void Game::handle_difficulty_select_input() {
    if (platform_->isKeyDown(KEY_A)) {
        set_difficulty(GameConstants::DIFFICULTY_EASY);
        if (selected_mode_ == 2) start_game(true, false);
        else if (selected_mode_ == 3) start_game(false, true);
    } else if (platform_->isKeyDown(KEY_B)) {
        set_difficulty(GameConstants::DIFFICULTY_MEDIUM);
        if (selected_mode_ == 2) start_game(true, false);
        else if (selected_mode_ == 3) start_game(false, true);
    } else if (platform_->isKeyDown(KEY_C)) {
        set_difficulty(GameConstants::DIFFICULTY_HARD);
        if (selected_mode_ == 2) start_game(true, false);
        else if (selected_mode_ == 3) start_game(false, true);
    } else if (platform_->isKeyDown(KEY_Q)) {
        set_game_state(GameConstants::GAME_MODE_SELECT);
    }
}

void Game::handle_game_input() {
    if (platform_->isKeyDown(KEY_0)) {
        set_game_state(GameConstants::GAME_MENU);
        return;
    }

    if (platform_->isKeyDown(KEY_ESC)) {
        quit_requested_ = true;
        return;
    }

    if (platform_->isKeyDown(KEY_S)) {
        SaveRecord record = SaveManager::createRecordFromGameState(
            board_->board_, board_->current_player_black_, move_phase_,
            board_->selected_row_, board_->selected_col_,
            chess_move_, arrow_move_,
            black_is_human_, white_is_human_,
            difficulty_,
            std::time(nullptr),
            current_game_id_);
        SaveManager::saveRecord(record);
        set_game_state(GameConstants::GAME_MENU);
        return;
    }

    if (board_->is_game_over()) {
        set_game_state(GameConstants::GAME_OVER);
        return;
    }

    bool is_current_player_black = board_->is_current_player_black();
    bool is_current_player_human = is_current_player_black ? black_is_human_ : white_is_human_;

    if (!is_current_player_human) {
        execute_ai_move();
        return;
    }

    handle_mouse_click();
}

void Game::handle_game_over_input() {
    if (platform_->isKeyDown(KEY_0)) {
        platform_->delayFPS(30);
        deleteCurrentGame();
        set_game_state(GameConstants::GAME_MENU);
    } else if (platform_->isKeyDown(KEY_ESC)) {
        quit_requested_ = true;
    }
}

void Game::handle_mouse_click() {
    if (!platform_->hasMouseMsg()) return;

    MouseEvent msg = platform_->getMouseMsg();
    if (!msg.left_down) return;

    if (!renderer_->is_click_on_board(msg.x, msg.y)) return;

    int row, col;
    if (!renderer_->screen_to_board(msg.x, msg.y, row, col)) return;

    switch (move_phase_) {
    case 0: handle_piece_selection(row, col); break;
    case 1: handle_piece_movement(row, col); break;
    case 2: handle_arrow_placement(row, col); break;
    }

    update_game_display();
}

void Game::handle_piece_selection(int row, int col) {
    GameConstants::CellState state = board_->get_cell_state(row, col);
    bool is_current_player_black = board_->is_current_player_black();

    if ((is_current_player_black && state == GameConstants::BLACK) ||
        (!is_current_player_black && state == GameConstants::WHITE)) {

        board_->set_selected_piece(row, col);
        chess_move_.from_row = row;
        chess_move_.from_col = col;

        std::vector<std::pair<int, int>> moves;
        board_->get_valid_moves(row, col, moves);

        move_phase_ = moves.empty() ? 0 : 1;
        update_game_display();
    }
}

void Game::handle_piece_movement(int row, int col) {
    GameConstants::CellState state = board_->get_cell_state(row, col);
    bool is_current_player_black = board_->is_current_player_black();

    if ((is_current_player_black && state == GameConstants::BLACK) ||
        (!is_current_player_black && state == GameConstants::WHITE)) {
        handle_piece_selection(row, col);
        return;
    }

    if (move_phase_ != 1) return;

    int selected_row, selected_col;
    board_->get_selected_piece(selected_row, selected_col);

    std::vector<std::pair<int, int>> valid_moves;
    board_->get_valid_moves(selected_row, selected_col, valid_moves);

    bool is_valid = false;
    for (const auto& move : valid_moves) {
        if (move.first == row && move.second == col) {
            is_valid = true;
            break;
        }
    }
    if (!is_valid) return;

    chess_move_.to_row = row;
    chess_move_.to_col = col;

    GameConstants::CellState piece_type = board_->is_current_player_black() ?
        GameConstants::BLACK : GameConstants::WHITE;

    board_->make_move(chess_move_, piece_type);

    move_phase_ = 2;
    arrow_move_.from_row = row;
    arrow_move_.from_col = col;

    update_game_display();
}

void Game::handle_arrow_placement(int row, int col) {
    std::vector<std::pair<int, int>> valid_arrows;
    board_->get_valid_moves(arrow_move_.from_row, arrow_move_.from_col, valid_arrows);

    bool is_valid = false;
    for (const auto& arrow : valid_arrows) {
        if (arrow.first == row && arrow.second == col) {
            is_valid = true;
            break;
        }
    }
    if (!is_valid) return;

    arrow_move_.to_row = row;
    arrow_move_.to_col = col;

    board_->make_move(arrow_move_, GameConstants::ARROW);

    move_phase_ = 0;
    chess_move_ = Move();
    arrow_move_ = Move();

    platform_->delayFPS(150);
    update_game_display();
}

void Game::execute_ai_move() {
    std::array<std::array<int, GameConstants::N>, GameConstants::N> board;
    bool is_black;
    board_->get_state(board, is_black);

    std::array<int, 6> move;
    switch (difficulty_) {
    case GameConstants::DIFFICULTY_EASY:
        move = getBestMove1(board, is_black);
        break;
    case GameConstants::DIFFICULTY_MEDIUM:
        move = getBestMove2(board, is_black);
        break;
    case GameConstants::DIFFICULTY_HARD:
        move = getBestMove3(board, is_black);
        break;
    default:
        move = getBestMove2(board, is_black);
        break;
    }

    if (move[0] == -1 && move[1] == -1) {
        board_->check_game_over();
        update_game_display();
        return;
    }

    Move chess_move;
    chess_move.from_row = move[0];
    chess_move.from_col = move[1];
    chess_move.to_row = move[2];
    chess_move.to_col = move[3];

    GameConstants::CellState piece_type = is_black ? GameConstants::BLACK : GameConstants::WHITE;
    board_->make_move(chess_move, piece_type);

    update_game_display();
    platform_->delayFPS(30);

    Move arrow_move;
    arrow_move.from_row = move[2];
    arrow_move.from_col = move[3];
    arrow_move.to_row = move[4];
    arrow_move.to_col = move[5];

    board_->make_move(arrow_move, GameConstants::ARROW);

    update_game_display();
    platform_->delayFPS(30);
}

void Game::update_highlights() {
    std::vector<std::pair<int, int>> old_moves = highlight_moves_;
    std::vector<std::pair<int, int>> old_arrows = highlight_arrows_;

    clear_highlights();

    if (move_phase_ == 1 && board_->has_selected_piece()) {
        int selected_row, selected_col;
        board_->get_selected_piece(selected_row, selected_col);
        board_->get_valid_moves(selected_row, selected_col, highlight_moves_);
    } else if (move_phase_ == 2) {
        board_->get_valid_moves(arrow_move_.from_row, arrow_move_.from_col, highlight_arrows_);
    }

    if (old_moves != highlight_moves_ || old_arrows != highlight_arrows_) {
        update_game_display();
    }
}

void Game::clear_highlights() {
    highlight_moves_.clear();
    highlight_arrows_.clear();
}

void Game::update_game_display() {
    update_highlights();
    renderer_->draw_board(*board_, highlight_moves_, highlight_arrows_);

    std::string current_player = board_->is_current_player_black() ? "黑方" : "白方";
    std::string game_status;

    switch (move_phase_) {
    case 0:
        if (board_->has_selected_piece()) {
            int selected_row, selected_col;
            board_->get_selected_piece(selected_row, selected_col);
            std::vector<std::pair<int, int>> moves;
            board_->get_valid_moves(selected_row, selected_col, moves);
            game_status = moves.empty() ? "该棋子无法移动" : "请选择或切换棋子";
        } else {
            game_status = "请选择棋子";
        }
        break;
    case 1:
        game_status = "请移动或切换棋子";
        break;
    case 2:
        game_status = "请放置障碍";
        break;
    }

    renderer_->draw_game_info(current_player, game_status, current_state_, board_->is_game_over());
}

void Game::enter_load_menu() {
    set_game_state(GameConstants::GAME_LOAD_MENU);
    load_menu_records_ = SaveManager::loadAllRecords();
    renderer_->draw_load_menu(load_menu_records_);
}

void Game::handle_load_input() {
    if (platform_->isKeyDown(KEY_0)) {
        set_game_state(GameConstants::GAME_MENU);
        return;
    }

    if (platform_->isKeyDown(KEY_ESC)) {
        quit_requested_ = true;
        return;
    }

    if (platform_->isKeyDown(KEY_DELETE)) {
        if (!load_menu_records_.empty()) {
            SaveManager::clearAllRecords();
            load_menu_records_.clear();
            renderer_->draw_load_menu(load_menu_records_);
        }
        return;
    }

    for (int i = 1; i <= 5; i++) {
        if (platform_->isKeyDown(KEY_1 + i - 1)) {
            if (i <= static_cast<int>(load_menu_records_.size())) {
                SaveRecord record = load_menu_records_[i - 1];

                current_game_id_ = record.game_id;
                board_->reset_game_status();
                board_->board_ = record.board;

                board_->current_player_black_ = record.current_player_black;
                board_->selected_row_ = record.selected_row;
                board_->selected_col_ = record.selected_col;

                move_phase_ = record.move_phase;
                chess_move_.from_row = record.chess_from_row;
                chess_move_.from_col = record.chess_from_col;
                chess_move_.to_row = record.chess_to_row;
                chess_move_.to_col = record.chess_to_col;
                arrow_move_.from_row = record.arrow_from_row;
                arrow_move_.from_col = record.arrow_from_col;
                arrow_move_.to_row = record.arrow_to_row;
                arrow_move_.to_col = record.arrow_to_col;

                black_is_human_ = record.black_is_human;
                white_is_human_ = record.white_is_human;
                difficulty_ = record.difficulty;

                clear_highlights();
                update_highlights();

                set_game_state(GameConstants::GAME_PLAYING);
            }
            return;
        }
    }
}

