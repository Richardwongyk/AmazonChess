#ifndef AI_KERNEL_H
#define AI_KERNEL_H

#include <array>
using Movement = std::array<int, 6>;
const int n = 8;

// 随机AI，简单难度
Movement getBestMove1(const std::array<std::array<int, n>, n>& board,
					 const bool is_black);

// Minimax搜索，中等难度
Movement getBestMove2(const std::array<std::array<int, n>, n>& board,
					 const bool is_black);

// 蒙特卡洛算法，较高难度
Movement getBestMove3(const std::array<std::array<int, n>, n>& board,
					 const bool is_black);


#endif // AI_KERNEL_H
