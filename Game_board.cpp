#include "game_board.h"
#include <array>

// 构造函数
GameBoard::GameBoard() : current_player_black_(true), game_over_(false), winner_black_(false),
selected_row_(-1), selected_col_(-1) {
	
	// 初始化棋盘大小
	board_.resize(GameConstants::N);
	for (int i = 0; i < GameConstants::N; ++i) {
		board_[i].resize(GameConstants::N, GameConstants::EMPTY);
	}
}



// 初始化棋盘
void GameBoard::initialize() {
	
	// 重置游戏状态
	current_player_black_ = true;  // 黑方先手
	game_over_ = false;
	winner_black_ = false;
	clear_selection();
	// 清空棋盘！
	board_.assign(GameConstants::N,
				  std::vector<GameConstants::CellState>(GameConstants::N, GameConstants::EMPTY));
		
	// 放置黑方棋子
	for (int i = 0; i < 4; i++) {
		int row = GameConstants::BLACK_AMAZONS[i][0];
		int col = GameConstants::BLACK_AMAZONS[i][1];
		board_[row][col] = GameConstants::BLACK;
	}
	
	// 放置白方棋子
	for (int i = 0; i < 4; i++) {
		int row = GameConstants::WHITE_AMAZONS[i][0];
		int col = GameConstants::WHITE_AMAZONS[i][1];
		board_[row][col] = GameConstants::WHITE;
	}
}

void GameBoard::reset_game_status() {
	game_over_ = false;
	winner_black_ = false;
	selected_row_ = -1;
	selected_col_ = -1;
	clear_selection();
}


// 获取棋盘状态
GameConstants::CellState GameBoard::get_cell_state(int row, int col) const {
	
	return board_[row][col];
}

// 切换当前玩家
void GameBoard::switch_player() {
	current_player_black_ = !current_player_black_;
}

// 设置选中的棋子
void GameBoard::set_selected_piece(int row, int col) {
	selected_row_ = row;
	selected_col_ = col;
}

// 获取选中的棋子
void GameBoard::get_selected_piece(int& row, int& col) const {
	row = selected_row_;
	col = selected_col_;
}

// 清除选中状态
void GameBoard::clear_selection() {
	selected_row_ = -1;
	selected_col_ = -1;
}

// 检查是否有棋子被选中
bool GameBoard::has_selected_piece() const {
	return selected_row_ >= 0 && selected_col_ >= 0;
}

// 获取棋子或障碍移动的所有可能位置
void GameBoard::get_valid_moves(int row, int col, std::vector<std::pair<int, int>>& moves) const {
	
	moves.clear();
	
	// 8个方向：上、下、左、右、左上、右上、左下、右下
	int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1},
		{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
	
	// 检查每个方向
	for (int dir = 0; dir < 8; dir++) {
		int row_step = directions[dir][0];
		int col_step = directions[dir][1];
		
		// 沿着该方向移动，直到遇到边界或障碍
		int current_row = row + row_step;
		int current_col = col + col_step;
		
		while (current_row >= 0 && current_row < GameConstants::N
				&& current_col >= 0 && current_col < GameConstants::N) {
			if (board_[current_row][current_col] == GameConstants::EMPTY) {
				moves.push_back(std::make_pair(current_row, current_col));
				current_row += row_step;
				current_col += col_step;
			} else {
				break;  // 遇到障碍，停止该方向的搜索
			}
		}
	}
}

// 执行移动
bool GameBoard::make_move(const Move& move, GameConstants::CellState chess) {
	
	board_[move.to_row][move.to_col] = chess;
	if (chess != GameConstants::ARROW) {	// 移动的是棋子
		board_[move.from_row][move.from_col] = GameConstants::EMPTY;
		clear_selection();	// 清除选中状态
	} else {		// 放置的是障碍
		// 切换玩家
		switch_player();
		
		// 检查游戏是否结束
		check_game_over();
	}
	
	return true;
}

// 获取指定玩家的所有棋子位置
void GameBoard::get_player_pieces(bool is_black, std::vector<std::pair<int, int>>& pieces) const {
	pieces.clear();
	
	GameConstants::CellState target_piece = is_black ? GameConstants::BLACK : GameConstants::WHITE;
	
	for (int row = 0; row < GameConstants::N; row++) {
		for (int col = 0; col < GameConstants::N; col++) {
			if (board_[row][col] == target_piece) {
				pieces.push_back(std::make_pair(row, col));
			}
		}
	}
}

// 判断游戏是否结束并设置结果
void GameBoard::check_game_over() {
	
	// 获取该玩家的所有棋子
	std::vector<std::pair<int, int>> pieces;
	get_player_pieces(current_player_black_, pieces);
	game_over_ = true;
	for (const auto& piece : pieces) {
		std::vector<std::pair<int, int>> moves;
		get_valid_moves(piece.first, piece.second, moves);
		
		if (!moves.empty()) {
			game_over_ = false;  // 找到可移动的棋子
			break;
		}
	}
	// 检查当前玩家是否还有可移动的棋子
	if (game_over_) {
		winner_black_ = !current_player_black_;  // 对方获胜
	}
}

// 导出GameState供AI使用
void GameBoard::get_state(std::array<std::array<int, GameConstants::N>, GameConstants::N>& board,
						  bool& is_black) const {
	for (int i = 0; i < GameConstants::N; ++i) {
		for (int j = 0; j < GameConstants::N; ++j) {
			board[i][j] = static_cast<int>(board_[i][j]);
		}
	}
	is_black = current_player_black_;
}
