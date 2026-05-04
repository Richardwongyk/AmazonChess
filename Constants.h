#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace GameConstants {
	// 棋盘相关常量
	const int N = 8;           // 8x8棋盘
	const int CELL_SIZE = 80;          // 每个格子的像素大小
	const int BOARD_MARGIN = 50;        // 棋盘边距
	const int BOARD_WIDTH = N * CELL_SIZE;  // 棋盘宽度
	const int BOARD_HEIGHT = N * CELL_SIZE; // 棋盘高度
	
	// 窗口大小设置
	const int WINDOW_WIDTH = BOARD_WIDTH + 2 * BOARD_MARGIN + 300;  // 留出右侧空间
	const int WINDOW_HEIGHT = BOARD_HEIGHT + 2 * BOARD_MARGIN;     // 留出上下边距
	
	// 棋子初始位置（黑方）
	const int BLACK_AMAZONS[4][2] = {
		{0, 2}, {2, 0}, {5, 0}, {7, 2}
	};
	
	// 棋子初始位置（白方）
	const int WHITE_AMAZONS[4][2] = {
		{0, 5}, {2, 7}, {5, 7}, {7, 5}
	};
	
	
	// 游戏状态枚举
	enum GameState {
		GAME_MENU,      // 菜单状态
		GAME_PLAYING,   // 游戏进行中
		GAME_OVER,       // 游戏结束
		GAME_LOAD_MENU,  // 加载菜单
		GAME_MODE_SELECT, //模式选择
		GAME_DIFFICULTY_SELECT // 难度选择
	};
	
	// 棋盘格子状态枚举
	enum CellState : int {
		EMPTY = 0,     // 空格子
		BLACK = 1,     // 黑棋
		WHITE = -1,     // 白棋
		ARROW = 2     // 障碍物
	};
	
	enum DifficultyLevel {
		DIFFICULTY_EASY = 1,   // 简单
		DIFFICULTY_MEDIUM = 2, // 中等
		DIFFICULTY_HARD = 3    // 困难
	};
}

#endif // CONSTANTS_H
