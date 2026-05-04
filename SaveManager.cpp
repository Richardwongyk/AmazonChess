#include "SaveManager.h"
#include "game_board.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>

const char* SaveManager::SAVE_FILE_NAME = "amazons_saves.dat";

std::vector<SaveRecord> SaveManager::loadAllRecords() {
	std::vector<SaveRecord> records;
	std::ifstream file(SAVE_FILE_NAME, std::ios::binary);
	
	if (!file.is_open()) {
		return records;
	}
	
	try {
		// 读取文件头
		uint32_t magic;
		uint16_t version;
		file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
		file.read(reinterpret_cast<char*>(&version), sizeof(version));
		
		if (magic != 0x414D5A4E || version != 1) {
			file.close();
			return records;
		}
		
		// 读取记录数量
		uint32_t count;
		file.read(reinterpret_cast<char*>(&count), sizeof(count));
		
		for (uint32_t i = 0; i < count && i < MAX_RECORDS; i++) {
			SaveRecord record;
			
			// 读取game_id
			size_t id_length;
			file.read(reinterpret_cast<char*>(&id_length), sizeof(id_length));
			if (id_length > 0) {
				record.game_id.resize(id_length);
				file.read(&record.game_id[0], id_length);
			}
			
			// 初始化棋盘为N×N大小
			record.board.resize(GameConstants::N);
			for (int i = 0; i < GameConstants::N; i++) {
				record.board[i].resize(GameConstants::N);
				for (int j = 0; j < GameConstants::N; j++) {
					int cell_value;
					file.read(reinterpret_cast<char*>(&cell_value), sizeof(int));
					record.board[i][j] = static_cast<GameConstants::CellState>(cell_value);
				}
			}
			
			
			// 读取其他字段
			file.read(reinterpret_cast<char*>(&record.current_player_black), sizeof(bool));
			file.read(reinterpret_cast<char*>(&record.move_phase), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.selected_row), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.selected_col), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.chess_from_row), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.chess_from_col), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.chess_to_row), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.chess_to_col), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.arrow_from_row), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.arrow_from_col), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.arrow_to_row), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.arrow_to_col), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.black_is_human), sizeof(bool));
			file.read(reinterpret_cast<char*>(&record.white_is_human), sizeof(bool));
			file.read(reinterpret_cast<char*>(&record.difficulty), sizeof(int));
			file.read(reinterpret_cast<char*>(&record.timestamp), sizeof(std::time_t));
			
			records.push_back(record);
		}
	}
	catch (const std::exception& e) {
		std::cout << "读取存档错误: " << e.what() << std::endl;
	}
	
	file.close();
	return records;
}

bool SaveManager::saveRecord(const SaveRecord& newRecord) {
	auto records = loadAllRecords();
	
	// 查找是否已存在相同game_id的记录
	bool found = false;
	for (auto& record : records) {
		if (record.game_id == newRecord.game_id && !record.game_id.empty()) {
			// 覆盖原有记录
			record = newRecord;
			found = true;
			break;
		}
	}
	
	if (!found) {
		// 如果是新棋局，插入到开头
		records.insert(records.begin(), newRecord);
		if (records.size() > MAX_RECORDS) {
			records.resize(MAX_RECORDS);
		}
	}
	
	std::ofstream file(SAVE_FILE_NAME, std::ios::binary);
	if (!file.is_open()) {
		return false;
	}
	
	try {
		// 写入文件头
		uint32_t magic = 0x414D5A4E;
		uint16_t version = 1;
		uint32_t count = static_cast<uint32_t>(records.size());
		
		file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
		file.write(reinterpret_cast<const char*>(&version), sizeof(version));
		file.write(reinterpret_cast<const char*>(&count), sizeof(count));
		
		for (const auto& r : records) {
			// 写入game_id
			size_t id_length = r.game_id.size();
			file.write(reinterpret_cast<const char*>(&id_length), sizeof(id_length));
			if (id_length > 0) {
				file.write(r.game_id.c_str(), id_length);
			}
			
			for (int i = 0; i < GameConstants::N; i++) {
				for (int j = 0; j < GameConstants::N; j++) {
					int cell_value = static_cast<int>(r.board[i][j]);
					file.write(reinterpret_cast<const char*>(&cell_value), sizeof(int));
				}
			}
			
			
			// 写入其他字段
			file.write(reinterpret_cast<const char*>(&r.current_player_black), sizeof(bool));
			file.write(reinterpret_cast<const char*>(&r.move_phase), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.selected_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.selected_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.chess_from_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.chess_from_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.chess_to_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.chess_to_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.arrow_from_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.arrow_from_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.arrow_to_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.arrow_to_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.black_is_human), sizeof(bool));
			file.write(reinterpret_cast<const char*>(&r.white_is_human), sizeof(bool));
			file.write(reinterpret_cast<const char*>(&r.difficulty), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.timestamp), sizeof(std::time_t));
		}
	}
	catch (const std::exception& e) {
		file.close();
		return false;
	}
	
	file.close();
	return true;
}

bool SaveManager::deleteRecord(int index) {
	if (index < 1) return false;
	
	auto records = loadAllRecords();
	if (index > static_cast<int>(records.size())) {
		return false;
	}
	
	records.erase(records.begin() + (index - 1));

	// Write to temp file then atomically replace
	const char* tmpName = "amazons_saves.tmp";
	std::ofstream file(tmpName, std::ios::binary);
	if (!file.is_open()) return false;

	try {
		uint32_t magic = 0x414D5A4E;
		uint16_t version = 1;
		uint32_t count = static_cast<uint32_t>(records.size());

		file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
		file.write(reinterpret_cast<const char*>(&version), sizeof(version));
		file.write(reinterpret_cast<const char*>(&count), sizeof(count));

		for (const auto& r : records) {
			size_t idLen = r.game_id.size();
			file.write(reinterpret_cast<const char*>(&idLen), sizeof(idLen));
			if (idLen > 0) file.write(r.game_id.c_str(), idLen);

			for (int i = 0; i < GameConstants::N; i++)
				for (int j = 0; j < GameConstants::N; j++) {
					int v = static_cast<int>(r.board[i][j]);
					file.write(reinterpret_cast<const char*>(&v), sizeof(int));
				}

			file.write(reinterpret_cast<const char*>(&r.current_player_black), sizeof(bool));
			file.write(reinterpret_cast<const char*>(&r.move_phase), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.selected_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.selected_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.chess_from_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.chess_from_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.chess_to_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.chess_to_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.arrow_from_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.arrow_from_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.arrow_to_row), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.arrow_to_col), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.black_is_human), sizeof(bool));
			file.write(reinterpret_cast<const char*>(&r.white_is_human), sizeof(bool));
			file.write(reinterpret_cast<const char*>(&r.difficulty), sizeof(int));
			file.write(reinterpret_cast<const char*>(&r.timestamp), sizeof(std::time_t));
		}
		file.close();

		// Atomic replacement
		std::remove(SAVE_FILE_NAME);
		std::rename(tmpName, SAVE_FILE_NAME);
		return true;
	} catch (...) {
		file.close();
		return false;
	}
}

bool SaveManager::clearAllRecords() {
	return std::remove(SAVE_FILE_NAME) == 0;
}

std::string SaveManager::formatTimestamp(std::time_t ts) {
	char buffer[40];
	std::tm* timeinfo = std::localtime(&ts);
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
	return std::string(buffer);
}

SaveRecord SaveManager::createRecordFromGameState(
			const std::vector<std::vector<GameConstants::CellState>>& board,
			bool current_player_black,
			int move_phase,
			int selected_row, int selected_col,
			const Move& chess_move,
			const Move& arrow_move,
			bool black_is_human, bool white_is_human,
			int difficulty,
			std::time_t timestamp,
			const std::string& game_id) {
	
	SaveRecord record;
	
	record.game_id = game_id;  // 设置游戏ID
	record.board = board;
	
	
	record.current_player_black = current_player_black;
	record.move_phase = move_phase;
	record.selected_row = selected_row;
	record.selected_col = selected_col;
	record.chess_from_row = chess_move.from_row;
	record.chess_from_col = chess_move.from_col;
	record.chess_to_row = chess_move.to_row;
	record.chess_to_col = chess_move.to_col;
	record.arrow_from_row = arrow_move.from_row;
	record.arrow_from_col = arrow_move.from_col;
	record.arrow_to_row = arrow_move.to_row;
	record.arrow_to_col = arrow_move.to_col;
	record.black_is_human = black_is_human;
	record.white_is_human = white_is_human;
	record.difficulty = difficulty;
	record.timestamp = timestamp;
	
	return record;
}
