#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <array>
#include <vector>
#include <string>
#include <ctime>
#include "Constants.h"

// 存档结构体
struct SaveRecord {
	
	std::string game_id;  // 游戏唯一标识
	
	// 棋盘状态
	std::vector<std::vector<GameConstants::CellState>> board;
	bool current_player_black;
	
	// 交互状态
	int move_phase;
	int selected_row, selected_col;
	int chess_from_row, chess_from_col, chess_to_row, chess_to_col;
	int arrow_from_row, arrow_from_col, arrow_to_row, arrow_to_col;
	
	// 游戏模式
	bool black_is_human, white_is_human;
	
	int difficulty;  // 1=简单, 2=中等, 3=困难
	
	// 时间戳
	std::time_t timestamp;
};

class SaveManager {
private:
	static const char* SAVE_FILE_NAME;
	static const int MAX_RECORDS = 100;
	
public:
	// 核心数据操作（静态函数，无状态）
	static std::vector<SaveRecord> loadAllRecords();
	static bool saveRecord(const SaveRecord& record);
	static bool deleteRecord(int index); // 1-based
	static bool clearAllRecords();
	
	// 工具函数
	static std::string formatTimestamp(std::time_t ts);
	static SaveRecord createRecordFromGameState(
				const std::vector<std::vector<GameConstants::CellState>>& board,
				bool current_player_black,
				int move_phase,
				int selected_row, int selected_col,
				const struct Move& chess_move,
				const struct Move& arrow_move,
				bool black_is_human, bool white_is_human,
				int difficulty,
				std::time_t timestamp,
				const std::string& game_id = ""
												);
};

#endif
