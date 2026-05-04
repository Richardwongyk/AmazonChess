#include "renderer.h"
#include "game_board.h"
#include "platform/IPlatform.h"
#include <iostream>

Renderer::Renderer(IPlatform* platform, int window_width, int window_height)
    : platform_(platform)
    , window_width_(window_width)
    , window_height_(window_height)
{
    board_x_ = (window_width_ - GameConstants::BOARD_WIDTH) / 2 - 100;
    board_y_ = (window_height_ - GameConstants::BOARD_HEIGHT) / 2;
}

bool Renderer::initialize() {
    if (!platform_->initWindow(window_width_, window_height_,
                                "亚马逊棋 - Amazon Chess Game")) {
        std::cerr << "图形窗口初始化失败！" << std::endl;
        return false;
    }
    return true;
}

void Renderer::clear_screen() {
    platform_->setBgColor(0x000000);  // BLACK
    platform_->clearDevice();
}

void Renderer::draw_board(const GameBoard& board,
                           const std::vector<std::pair<int, int>>& highlight_moves,
                           const std::vector<std::pair<int, int>>& highlight_arrows) {
    clear_screen();
    draw_board_border();

    for (int row = 0; row < GameConstants::N; row++) {
        for (int col = 0; col < GameConstants::N; col++) {
            bool is_highlighted = false;
            bool is_move_highlight = true;

            for (const auto& pos : highlight_moves) {
                if (pos.first == row && pos.second == col) {
                    is_highlighted = true;
                    is_move_highlight = true;
                    break;
                }
            }

            if (!is_highlighted) {
                for (const auto& pos : highlight_arrows) {
                    if (pos.first == row && pos.second == col) {
                        is_highlighted = true;
                        is_move_highlight = false;
                        break;
                    }
                }
            }

            draw_cell_background(row, col, is_highlighted, is_move_highlight);
        }
    }

    for (int row = 0; row < GameConstants::N; row++) {
        for (int col = 0; col < GameConstants::N; col++) {
            GameConstants::CellState state = board.get_cell_state(row, col);
            if (state != GameConstants::EMPTY) {
                draw_piece(row, col, state);
            }
        }
    }

    if (board.has_selected_piece()) {
        int selected_row, selected_col;
        board.get_selected_piece(selected_row, selected_col);
        draw_selection(selected_row, selected_col);
    }

    platform_->flushWindow();
}

void Renderer::draw_cell_background(int row, int col, bool is_highlighted, bool is_move_highlight) {
    int screen_x, screen_y;
    board_to_screen(row, col, screen_x, screen_y);

    unsigned int base_color;
    if ((row + col) % 2 == 0) {
        base_color = 0xF5DEB3;
    } else {
        base_color = 0x8B4513;
    }

    platform_->setFillColor(base_color);
    platform_->fillRect(screen_x, screen_y,
                        GameConstants::CELL_SIZE, GameConstants::CELL_SIZE);

    if (is_highlighted) {
        int base_r = (base_color >> 16) & 0xFF;
        int base_g = (base_color >> 8) & 0xFF;
        int base_b = base_color & 0xFF;

        int highlight_r, highlight_g, highlight_b;
        if (is_move_highlight) {
            highlight_r = 0x00;
            highlight_g = 0xFF;
            highlight_b = 0x00;
        } else {
            highlight_r = 0x00;
            highlight_g = 0xFF;
            highlight_b = 0xFF;
        }

        int mixed_r = (highlight_r * 0.2f) + (base_r * 0.8f);
        int mixed_g = (highlight_g * 0.2f) + (base_g * 0.8f);
        int mixed_b = (highlight_b * 0.2f) + (base_b * 0.8f);

        if (mixed_r > 255) mixed_r = 255;
        if (mixed_g > 255) mixed_g = 255;
        if (mixed_b > 255) mixed_b = 255;

        unsigned int mixed_color = (mixed_r << 16) | (mixed_g << 8) | mixed_b;

        platform_->setFillColor(mixed_color);
        platform_->fillRect(screen_x, screen_y,
                            GameConstants::CELL_SIZE, GameConstants::CELL_SIZE);
    }

    platform_->setLineColor(0x654321);
    platform_->drawRect(screen_x, screen_y,
                        screen_x + GameConstants::CELL_SIZE,
                        screen_y + GameConstants::CELL_SIZE);
}

void Renderer::draw_board_border() {
    platform_->setLineColor(0x654321);
    platform_->setFillColor(0x654321);

    int border_width = 3;
    int bw = border_width;

    platform_->fillRect(board_x_ - bw, board_y_ - bw,
                        GameConstants::BOARD_WIDTH + 2 * bw, bw);
    platform_->fillRect(board_x_ - bw,
                        board_y_ + GameConstants::BOARD_HEIGHT,
                        GameConstants::BOARD_WIDTH + 2 * bw, bw);
    platform_->fillRect(board_x_ - bw, board_y_, bw, GameConstants::BOARD_HEIGHT);
    platform_->fillRect(board_x_ + GameConstants::BOARD_WIDTH, board_y_,
                        bw, GameConstants::BOARD_HEIGHT);
}

void Renderer::draw_piece(int row, int col, GameConstants::CellState state) {
    int screen_x, screen_y;
    board_to_screen(row, col, screen_x, screen_y);

    int center_x = screen_x + GameConstants::CELL_SIZE / 2;
    int center_y = screen_y + GameConstants::CELL_SIZE / 2;
    int piece_size = GameConstants::CELL_SIZE / 3;

    platform_->enableAntiAlias(true);

    switch (state) {
    case GameConstants::BLACK:
        platform_->setFillColor(0x2F4F4F);
        platform_->setLineColor(0x1A1A1A);
        platform_->fillCircle(center_x, center_y, piece_size);

        platform_->setFillColor(0x1A1A1A);
        platform_->fillCircle(center_x + 1, center_y + 1, piece_size - 1);

        platform_->setFillColor(0x2F4F4F);
        platform_->fillCircle(center_x, center_y, piece_size);

        platform_->setFillColor(0x708090);
        platform_->fillCircle(center_x - piece_size / 3,
                              center_y - piece_size / 3,
                              piece_size / 3);
        break;

    case GameConstants::WHITE: {
        int main_radius = piece_size;

        platform_->setFillColor(0xC0C0C0);
        platform_->setLineColor(0xA0A0A0);
        platform_->fillCircle(center_x + 2, center_y + 2, main_radius);

        platform_->setFillColor(0xF5F5F5);
        platform_->setLineColor(0xD0D0D0);
        platform_->fillCircle(center_x, center_y, main_radius);

        platform_->setFillColor(0xFFFFFF);
        platform_->setLineColor(0xFFFFFF);

        int highlight_width = main_radius * 0.6;
        int highlight_height = main_radius * 0.4;
        platform_->fillEllipse(center_x - main_radius / 3,
                               center_y - main_radius / 3,
                               highlight_width, highlight_height);

        platform_->setFillColor(0xFFFFFF);
        platform_->fillCircle(center_x - main_radius / 4,
                              center_y - main_radius / 4,
                              main_radius / 6);

        platform_->setLineColor(0xC0C0C0);
        platform_->setLineStyle(LINE_SOLID, 2);
        platform_->drawCircle(center_x, center_y, main_radius + 1);

        platform_->setLineStyle(LINE_SOLID, 1);
        break;
    }

    case GameConstants::ARROW: {
        platform_->setLineColor(0xDC143C);
        platform_->setLineStyle(LINE_SOLID, 3);

        int base_size = piece_size * 0.7;

        platform_->setFillColor(0xFFE4E1);
        platform_->setLineColor(0xDC143C);
        platform_->fillCircle(center_x, center_y, base_size + 4);

        platform_->setLineColor(0x8B0000);
        platform_->setLineStyle(LINE_SOLID, 3);

        platform_->drawLine(center_x - base_size, center_y - base_size,
                            center_x + base_size, center_y + base_size);
        platform_->drawLine(center_x + base_size, center_y - base_size,
                            center_x - base_size, center_y + base_size);

        platform_->setLineStyle(LINE_SOLID, 1);
        break;
    }

    default:
        break;
    }
}

void Renderer::draw_selection(int row, int col) {
    int screen_x, screen_y;
    board_to_screen(row, col, screen_x, screen_y);

    int center_x = screen_x + GameConstants::CELL_SIZE / 2;
    int center_y = screen_y + GameConstants::CELL_SIZE / 2;
    int selection_size = GameConstants::CELL_SIZE / 3 + 5;

    platform_->setLineColor(0xFFFF00);
    platform_->setLineStyle(LINE_SOLID, 3);
    platform_->drawCircle(center_x, center_y, selection_size);

    platform_->setLineStyle(LINE_SOLID, 1);
}

void Renderer::draw_game_info(const std::string& current_player,
                               const std::string& game_status,
                               GameConstants::GameState current_state,
                               bool is_game_over) {
    platform_->setLineColor(0xFFFFFF);

    int info_x = board_x_ + GameConstants::BOARD_WIDTH + 20;
    int info_y = board_y_ + 20;

    platform_->setFont(24, "仿宋");

    platform_->drawText(info_x, info_y, ("当前玩家: " + current_player).c_str());
    platform_->drawText(info_x, info_y + 40, ("状态: " + game_status).c_str());

    int y_offset = 80;

    if (current_state == GameConstants::GAME_PLAYING) {
        if (!is_game_over) {
            platform_->drawText(info_x, info_y + y_offset, "操作说明:");
            platform_->drawText(info_x, info_y + y_offset + 30, "1. 点击选择棋子");
            platform_->drawText(info_x, info_y + y_offset + 60, "2. 点击移动棋子");
            platform_->drawText(info_x, info_y + y_offset + 90, "3. 点击放置障碍");
            platform_->drawText(info_x, info_y + y_offset + 140, "快捷键:");
            platform_->drawText(info_x, info_y + y_offset + 170, "按S 保存并返回菜单");
            platform_->drawText(info_x, info_y + y_offset + 200, "按0 不保存返回菜单");
            platform_->drawText(info_x, info_y + y_offset + 230, "ESC退出游戏");
        } else {
            platform_->drawText(info_x, info_y + y_offset, "游戏已结束");
            platform_->drawText(info_x, info_y + y_offset + 30, "按0返回菜单");
            platform_->drawText(info_x, info_y + y_offset + 60, "ESC退出游戏");
        }
    }
}

void Renderer::draw_menu() {
    clear_screen();

    platform_->setLineColor(0xFFFFFF);

    int title_x = window_width_ / 2 - 130;
    int title_y = window_height_ / 4;

    platform_->setFont(55, "仿宋");
    platform_->drawText(title_x, title_y, "亚马逊棋");

    platform_->setFont(36, "仿宋");

    int menu_x = window_width_ / 2 - 130;
    int menu_start_y = title_y + 150;

    platform_->drawText(menu_x, menu_start_y, "A. 新游戏");
    platform_->drawText(menu_x, menu_start_y + 60, "B. 载入游戏");
    platform_->drawText(menu_x, menu_start_y + 120, "C. 退出游戏");

    platform_->setFont(24, "仿宋");
    platform_->drawText(menu_x, menu_start_y + 220, "请按相应字母选择...");
}

void Renderer::draw_load_menu(const std::vector<SaveRecord>& records) {
    clear_screen();
    platform_->setLineColor(0xFFFFFF);

    int start_y = 50;
    int line_height = 40;

    if (records.empty()) {
        platform_->setFont(27, "仿宋");
        platform_->drawText(100, start_y, "暂无对局，按0返回菜单，按ESC退出游戏");
    } else {
        platform_->setFont(36, "仿宋");
        platform_->drawText(100, start_y, "最近储存的对局：");
        platform_->setFont(27, "仿宋");
        start_y += 40;

        for (size_t i = 0; i < records.size() && i < 5; i++) {
            const auto& record = records[i];

            std::string difficulty_str;
            switch (record.difficulty) {
            case GameConstants::DIFFICULTY_EASY:
                difficulty_str = "简单";
                break;
            case GameConstants::DIFFICULTY_MEDIUM:
                difficulty_str = "中等";
                break;
            case GameConstants::DIFFICULTY_HARD:
                difficulty_str = "困难";
                break;
            default:
                difficulty_str = "中等";
                break;
            }
            bool all_human = record.black_is_human && record.white_is_human;

            std::string info_1 = std::to_string(i + 1)
                + ". 黑方:"
                + (record.black_is_human ? "玩家" : "AI")
                + "  白方:"
                + (record.white_is_human ? "玩家" : "AI")
                + (!all_human ? "    难度: " + difficulty_str : "");

            start_y += line_height;
            platform_->drawText(50, start_y, info_1.c_str());

            std::string info_2 = "   时间:"
                + SaveManager::formatTimestamp(record.timestamp)
                + "   轮到" + std::string(record.current_player_black ? "黑方" : "白方")
                + (record.move_phase == 0 ? "-选择棋子" :
                   record.move_phase == 1 ? "-移动棋子" : "-放置障碍");

            start_y += line_height;
            platform_->drawText(50, start_y, info_2.c_str());

            start_y += line_height / 2;
        }

        start_y += 1.5 * line_height;
        platform_->setFont(24, "仿宋");
        platform_->drawText(50, start_y,
                            "请按数字键选择, 按0返回菜单，按ESC退出游戏，按Delete清空历史");
    }

    platform_->flushWindow();
}

void Renderer::draw_mode_select() {
    clear_screen();

    int title_x = window_width_ / 2 - 130;
    int title_y = window_height_ / 4;
    platform_->setFont(48, "仿宋");
    platform_->drawText(title_x, title_y, "模式选择");
    int menu_x = window_width_ / 2 - 130;
    int menu_start_y = title_y + 160;
    platform_->setFont(30, "仿宋");
    platform_->drawText(menu_x, menu_start_y, "1. 人人对战");
    platform_->drawText(menu_x, menu_start_y + 60, "2. 人机对战（您是黑方）");
    platform_->drawText(menu_x, menu_start_y + 120, "3. 人机对战（您是白方）");
    platform_->drawText(menu_x, menu_start_y + 180, "0. 返回菜单");
}

void Renderer::draw_difficulty_select() {
    clear_screen();

    platform_->setLineColor(0xFFFFFF);

    int title_x = window_width_ / 2 - 140;
    int title_y = window_height_ / 4;

    platform_->setFont(48, "仿宋");
    platform_->drawText(title_x, title_y, "选择难度");

    platform_->setFont(36, "仿宋");

    int menu_x = window_width_ / 2 - 200;
    int menu_start_y = title_y + 160;

    platform_->drawText(menu_x, menu_start_y, "A. 简单————新手推荐");
    platform_->drawText(menu_x, menu_start_y + 60, "B. 中等————标准挑战");
    platform_->drawText(menu_x, menu_start_y + 120, "C. 困难————高手对决");
    platform_->drawText(menu_x, menu_start_y + 200, "Q. 返回上一步");

    platform_->setFont(24, "仿宋");
    platform_->drawText(menu_x, menu_start_y + 270, "请按相应字母选择...");

    platform_->flushWindow();
}

void Renderer::draw_game_over(const std::string& winner) {
    int dialog_width = 480;
    int dialog_height = 350;
    int dialog_x = (window_width_ - dialog_width) / 2;
    int dialog_y = (window_height_ - dialog_height) / 2;

    // Semi-transparent overlay
    platform_->setFillColor(0x80000000);
    platform_->alphaFillRect(0, 0, window_width_, window_height_);

    // Dialog background
    platform_->setFillColor(0x1E3A8A);
    platform_->setLineColor(0xFFD700);
    platform_->setLineStyle(LINE_SOLID, 3);
    platform_->fillRect(dialog_x, dialog_y, dialog_width, dialog_height);
    platform_->drawRect(dialog_x, dialog_y,
                        dialog_x + dialog_width, dialog_y + dialog_height);

    // Inner border
    platform_->setLineColor(0x3B82F6);
    platform_->setLineStyle(LINE_SOLID, 2);
    int inner_margin = 10;
    platform_->drawRect(dialog_x + inner_margin, dialog_y + inner_margin,
                        dialog_x + dialog_width - inner_margin,
                        dialog_y + dialog_height - inner_margin);

    platform_->setTextBgMode(TEXT_BG_TRANSPARENT);

    int text_y_start = dialog_y + 60;

    platform_->setFont(48, "仿宋");
    platform_->setLineColor(0xFFD700);

    const char* title = "游戏结束!";
    int title_width = platform_->textWidth(title);
    int title_x = dialog_x + (dialog_width - title_width) / 2;
    platform_->drawText(title_x, text_y_start, title);

    platform_->setFont(36, "仿宋");
    platform_->setLineColor(0xFFFFFF);

    std::string winner_text = winner + " 获胜!";
    int winner_width = platform_->textWidth(winner_text.c_str());
    int winner_x = dialog_x + (dialog_width - winner_width) / 2;
    platform_->drawText(winner_x, text_y_start + 80, winner_text.c_str());

    platform_->setLineColor(0x3B82F6);
    platform_->drawLine(dialog_x + 80, text_y_start + 140,
                        dialog_x + dialog_width - 80, text_y_start + 140);

    platform_->setFont(24, "宋体");
    platform_->setLineColor(0xE0E0E0);

    const char* hint1 = "按 0 返回主菜单";
    const char* hint2 = "按 ESC 退出游戏";

    int hint1_width = platform_->textWidth(hint1);
    int hint2_width = platform_->textWidth(hint2);
    int hint_x = dialog_x + (dialog_width - hint1_width) / 2;

    platform_->drawText(hint_x, text_y_start + 170, hint1);

    hint_x = dialog_x + (dialog_width - hint2_width) / 2;
    platform_->drawText(hint_x, text_y_start + 200, hint2);

    platform_->setTextBgMode(TEXT_BG_OPAQUE);

    platform_->flushWindow();
}

void Renderer::board_to_screen(int row, int col, int& screen_x, int& screen_y) {
    screen_x = board_x_ + col * GameConstants::CELL_SIZE;
    screen_y = board_y_ + row * GameConstants::CELL_SIZE;
}

bool Renderer::screen_to_board(int screen_x, int screen_y, int& row, int& col) {
    col = (screen_x - board_x_) / GameConstants::CELL_SIZE;
    row = (screen_y - board_y_) / GameConstants::CELL_SIZE;

    if (row < 0 || row >= GameConstants::N ||
        col < 0 || col >= GameConstants::N) {
        return false;
    }
    return true;
}

bool Renderer::is_click_on_board(int screen_x, int screen_y) {
    return (screen_x >= board_x_ && screen_x < board_x_ + GameConstants::BOARD_WIDTH &&
            screen_y >= board_y_ && screen_y < board_y_ + GameConstants::BOARD_HEIGHT);
}
