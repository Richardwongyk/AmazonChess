#ifndef GAME_BOARD_H
#define GAME_BOARD_H

#include "constants.h"
#include <vector>
#include <string>
#include <array>
#include <utility>

/**
 * 移动结构体 - 表示棋子或障碍的一步完整移动，二者地位相同
 */

struct Move {
	int from_row, from_col;    // 起始位置
	int to_row, to_col;        // 目标位置
};

/**
 * 棋盘类 - 管理游戏状态和规则
 */
class GameBoard {
private:
	// 棋盘状态二维数组
	std::vector<std::vector<GameConstants::CellState>> board_;
	
	// 当前玩家（true为黑方，false为白方）
	bool current_player_black_;
	
	// 游戏是否结束
	bool game_over_;
	
	// 获胜者（true为黑方，false为白方）
	bool winner_black_;
	
	// 选中的棋子位置
	int selected_row_, selected_col_;
	
	// 获取指定玩家的所有棋子位置
	void get_player_pieces(bool is_black, std::vector<std::pair<int, int>>& pieces) const;
	
public:
	
	friend class Game;
	friend class SaveManager;
	// 构造函数
	GameBoard();
	
	// 析构函数
	~GameBoard() {};
	
	// 初始化棋盘
	void initialize();
	
	void reset_game_status();  // 重置游戏结束相关状态
	
	// 获取棋盘状态
	GameConstants::CellState get_cell_state(int row, int col) const;
	
	// 获取当前玩家
	bool is_current_player_black() const { return current_player_black_; }
	
	// 切换当前玩家
	void switch_player();
	
	// 检查游戏是否结束
	bool is_game_over() const { return game_over_; }
	
	// 获取获胜者
	bool is_winner_black() const { return winner_black_; }
	
	// 设置选中的棋子
	void set_selected_piece(int row, int col);
	
	// 获取选中的棋子
	void get_selected_piece(int& row, int& col) const;
	
	// 清除选中状态
	void clear_selection();
	
	// 检查是否有棋子被选中
	bool has_selected_piece() const;
	
	// 执行移动
	bool make_move(const Move& move, GameConstants::CellState chess);
	
	// 获取指定棋子的所有可移动位置
	void get_valid_moves(int row, int col, std::vector<std::pair<int, int>>& moves) const;
	
	// 判断游戏是否结束并设置结果
	void check_game_over();
	
	// 导出GameState供AI使用
	void get_state(std::array<std::array<int, GameConstants::N>, GameConstants::N>& board, bool& is_black) const;
	
};

#endif // GAME_BOARD_H
