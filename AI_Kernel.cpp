#include "AI_Kernel.h"
#include <vector>
#include <array>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <queue>
#include <cmath>
#include <chrono>

/*
*	const int n == 8
*	Movement == std::array<int, 6>
*/
// 简单版本：随机算法
Movement getBestMove1(const std::array<std::array<int, n>, n>& board,
					 const bool is_black) {
	
	int my_piece = is_black ? 1 : -1;
	
	// 收集所有合法走法
	static std::vector<Movement> valid_moves;
	
	valid_moves.clear();
	
	constexpr int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
	constexpr int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
	
	// 遍历所有格子找自己的棋子
	for (int x = 0; x < n; x++) {
		for (int y = 0; y < n; y++) {
			if (board[x][y] == my_piece) {
				// 找到棋子，获取它的所有走法
				for (int dir = 0; dir < 8; dir++) {
					int nx = x + dx[dir];
					int ny = y + dy[dir];
					
					// 沿该方向探索所有可达位置
					while (nx >= 0 && nx < n && ny >= 0 && ny < n && board[nx][ny] == 0) {
						
						//先添加原位置
						valid_moves.emplace_back(std::array{x, y, nx, ny, x, y});
						
						// 从(nx,ny)发射障碍
						for (int arrow_dir = 0; arrow_dir < 8; arrow_dir++) {
							int ax = nx + dx[arrow_dir];
							int ay = ny + dy[arrow_dir];
							
							while (ax >= 0 && ax < n && ay >= 0 && ay < n && board[ax][ay] == 0) {
								valid_moves.emplace_back(std::array{x, y, nx, ny, ax, ay});
								ax += dx[arrow_dir];
								ay += dy[arrow_dir];
							}
							
						}
						
						nx += dx[dir];
						ny += dy[dir];
					}
				}
			}
		}
	}
	
	// 随机选择一个走法
	if (!valid_moves.empty()) {
		return valid_moves[rand() % valid_moves.size()];
	}
	
	// 无合法走法（理论上不应该发生）
	return {-1, -1, -1, -1, -1, -1};
}

// 中等难度版本：Minimax算法
Movement getBestMove2(const std::array<std::array<int, n>, n>& board,
					  const bool is_black) {
	// 方向数组定义（8个移动方向）
	constexpr int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
	constexpr int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
	
	// 当前玩家与对手的棋子标识
	const int my_piece = is_black ? 1 : -1;
	const int opp_piece = -my_piece;  // 明确对手棋子标识
	
	// 棋局评估函数
	auto evaluate = [&](const std::array<std::array<int, n>, n>& b) -> int {
		int score = 0;
		bool my_area[64] = {}, opp_area[64] = {};  // 领地标记数组
		int my_center = 0, opp_center = 0;        // 中心价值计算
		int my_block = 0, opp_block = 0;          // 领地块数统计
		
		// 遍历棋盘每个位置进行评估
		for (int i = 0; i < 64; i++) {
			int x = i >> 3, y = i & 7;  // 坐标转换（行号、列号）
			int p = b[x][y];            // 当前棋子类型
			
			// 仅处理当前玩家的棋子或对手棋子
			if (p != my_piece && p != opp_piece) continue;
			
			int mobility = 0, directions = 0, safety_penalty = 0;
			
			// 计算棋子活动性（Mobility）和安全性
			for (int d = 0; d < 8; d++) {
				int nx = x + dx[d], ny = y + dy[d];
				int len = 0;  // 可移动步长计数
				
				// 计算当前方向的可移动空间
				while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && b[nx][ny] == 0) {
					len++;
					mobility++;  // 活动性计数
					nx += dx[d];
					ny += dy[d];
				}
				
				// 方向有效性统计和安全惩罚
				if (len > 0) directions++;
				if (len <= 1) safety_penalty -= 30;  // 死路惩罚机制
			}
			
			// 更新棋局得分（加权计算，自己的减去对手的）
			score += (p == my_piece) ?
				(mobility * 12 + directions * 8 + safety_penalty) :
				-(mobility * 12 + directions * 8 + safety_penalty);
			
			// 领地标记（向四个方向扩展）
			for (int d = 0; d < 8; d++) {
				int nx = x + dx[d], ny = y + dy[d];
				while (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && b[nx][ny] == 0) {
					// 标记领地归属
					(p == my_piece ? my_area : opp_area)[(nx << 3) + ny] = true;
					nx += dx[d];
					ny += dy[d];
				}
			}
			
			// 中心价值计算（距离中心越近价值越高）
			int center_dist = 7 - (abs(x - 3) + abs(y - 3));
			if (p == my_piece) my_center += center_dist;
			else opp_center += center_dist;
		}
		
		// 领地价值计算（独占区域奖励）
		for (int i = 0; i < 64; i++) {
			if (my_area[i] && !opp_area[i]) score += 25;  // 独占区域奖励
			else if (opp_area[i] && !my_area[i]) score -= 25;
		}
		
		// 中心位置加权（强化中心控制）
		score += (my_center - opp_center) * 5;
		
		// 封锁对手奖励（主动阻断对手移动路径）
		for (int i = 0; i < 64; i++) {
			if (b[i >> 3][i & 7] == 2) {  // 检查障碍物
				for (int d = 0; d < 8; d++) {
					int nx = (i >> 3) + dx[d], ny = (i & 7) + dy[d];
					if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8 && b[nx][ny] == opp_piece) {
						score += 10;  // 封锁对手棋子奖励
					}
				}
			}
		}
		return score;
	};
	
	// 2. 生成所有合法移动（广度优先搜索）
	struct Move {
		int x, y;    // 起始位置
		int nx, ny;  // 新位置
		int ax, ay;  // 障碍放置位置
	};
	Move moves[2048];  // 最大可能移动数
	int move_cnt = 0;  // 实际移动计数
	
	// 遍历棋盘生成所有合法移动
	for (int sx = 0; sx < 8; sx++) {
		for (int sy = 0; sy < 8; sy++) {
			if (board[sx][sy] != my_piece) continue;  // 仅处理当前玩家棋子
			
			// 八个方向遍历
			for (int d1 = 0; d1 < 8; d1++) {
				int mx = sx + dx[d1], my = sy + dy[d1];
				
				// 检查中间点是否为空，棋子可放置位置
				while (mx >= 0 && mx < 8 && my >= 0 && my < 8 && board[mx][my] == 0) {
					// 障碍目标点遍历
					for (int d2 = 0; d2 < 8; d2++) {
						int ax = mx + dx[d2], ay = my + dy[d2];
						
						// 检查目标点合法性（允许障碍放到到空位或己方起点）
						while (ax >= 0 && ax < 8 && ay >= 0 && ay < 8 &&
							   (board[ax][ay] == 0 || (ax == sx && ay == sy))) {
							
							// 记录移动
							moves[move_cnt++] = {sx, sy, mx, my, ax, ay};
							
							// 防止内存溢出
							if (move_cnt >= 2048) goto end_gen;
							
							// 继续向同方向延伸
							ay += dy[d2];
							ax += dx[d2];
						}
					}
					// 更新到下一个落子位置
					my += dy[d1];
					mx += dx[d1];
				}
			}
		}
	}
	end_gen:
	// 无合法移动时返回无效值
	if (move_cnt == 0) return Movement{-1, -1, -1, -1, -1, -1};
	
	// 3. 候选走法排序（插入排序优化）
	struct Rated { Move m; int s; };  // 移动+评分结构体
	Rated top[100];                  // 保留前100候选
	int top_cnt = 0;                 // 候选计数
	
	// 遍历所有移动进行初步评估
	for (int i = 0; i < move_cnt; i++) {
		auto& m = moves[i];
		
		// 创建临时棋盘模拟移动
		std::array<std::array<int, n>, n> b = board;
		b[m.x][m.y] = 0;        // 清除原位置
		b[m.nx][m.ny] = my_piece; // 设置新位置
		b[m.ax][m.ay] = 2;      // 设置障碍位置
		
		// 计算移动评分
		int s = evaluate(b);
		
		// 插入排序维护前100最佳移动
		int j = top_cnt;
		while (j > 0 && s > top[j-1].s) j--;
		if (j < 100) {
			// 后移元素腾出位置
			for (int k = std::min(99, top_cnt); k > j; k--)
				top[k] = top[k-1];
			top[j] = {m, s};  // 插入新元素
			top_cnt = std::min(top_cnt + 1, 100);
		}
	}
	
	// 4. 深度2搜索
	int best_idx = 0, best_score = -1000000;
	for (int i = 0; i < top_cnt && i < 30; i++) {  // 仅验证前30候选
		auto& m1 = top[i].m;
		
		// 创建模拟棋盘
		std::array<std::array<int, n>, n> b1 = board;
		b1[m1.x][m1.y] = 0;
		b1[m1.nx][m1.ny] = my_piece;
		b1[m1.ax][m1.ay] = 2;
		
		// 模拟对手最佳回应（Minimax逻辑）
		int opp_best = -1000000;
		for (int j = 0; j < move_cnt && j < 200; j++) {  // 限制对手搜索宽度
			auto& m2 = moves[j];
			if (b1[m2.x][m2.y] != opp_piece) continue;  // 仅处理对手棋子
			
			// 创建对手模拟棋盘
			std::array<std::array<int, n>, n> b2 = b1;
			b2[m2.x][m2.y] = 0;        // 清除原位置
			b2[m2.nx][m2.ny] = opp_piece; // 设置对手落子位置
			b2[m2.ax][m2.ay] = 2;      // 设置对手放置障碍处
			
			// 递归评估对手走法
			opp_best = std::max(opp_best, evaluate(b2));
		}
		
		// Minimax决策：当前得分 - 对手最佳回应
		int current_score = top[i].s - opp_best;
		if (current_score > best_score) {
			best_score = current_score;
			best_idx = i;
		}
	}
	
	// 返回最佳移动方案
	return Movement{
		top[best_idx].m.x, top[best_idx].m.y,
		top[best_idx].m.nx, top[best_idx].m.ny,
		top[best_idx].m.ax, top[best_idx].m.ay
	};
}


namespace AmazonGame {
	
// ==================== 核心数据结构 ====================
	
	// 动作结构：完整的一回合 = 皇后移动 + 射箭
	// x0,y0: 皇后原位置；x1,y1: 皇后目标位置；x2,y2: 箭落位置
	struct Action {
		int x0, y0, x1, y1, x2, y2;
		
		explicit Action(int x0 = -1, int y0 = -1, int x1 = -1, int y1 = -1, int x2 = -1, int y2 = -1)
		: x0(x0), y0(y0), x1(x1), y1(y1), x2(x2), y2(y2) {}
		
		// 提取移动部分（不包含射箭）
		const Action getMove() const { return Action(x0, y0, x1, y1, -1, -1); }
		// 提取射箭部分（移动目标位置作为射箭起点）
		const Action getArrow() const { return Action(-1, -1, x1, y1, x2, y2); }
		
		bool isMove() const { return x0 != -1; }  // 是否为移动动作
		bool isArrow() const { return x2 != -1; }  // 是否为射箭动作
		bool isEmpty() const { return x1 == -1; }  // 是否为空动作（无效）
		
		// 合并移动和射箭为完整动作
		friend inline Action operator*(const Action &move, const Action &arrow) {
			return Action(move.x0, move.y0, move.x1, move.y1, arrow.x2, arrow.y2);
		}
		
		friend inline bool operator==(const Action &a, const Action &b) {
			return a.x0 == b.x0 && a.y0 == b.y0 && a.x1 == b.x1 && a.y1 == b.y1 && a.x2 == b.x2 && a.y2 == b.y2;
		}
	};
	
// ==================== 棋盘类 ====================
	
	const int gridSize = 8;  // 棋盘大小
	const int dx[8] = {-1, -1, -1, 0, 0, 1, 1, 1};  // 8个方向x增量
	const int dy[8] = {-1, 0, 1, -1, 1, -1, 0, 1};  // 8个方向y增量
	
	inline bool coordValid(int x, int y) {
		return x >= 0 && x < gridSize && y >= 0 && y < gridSize;
	}
	
	enum {
		Empty = -1,  // 空格
		Black = 0,   // 黑方棋子
		White = 1,   // 白方棋子
		Arrow = 2    // 箭（障碍）
	};
	
	class ChessBoard {
	public:
		int turn;  // 当前回合数
		int color; // 当前玩家颜色
		int grid[gridSize][gridSize];  // 棋盘格子状态
		Action acts[60];  // 历史动作记录（最多60步）
		int chessX[2][4]; // 两方棋子X坐标 [颜色][棋子ID]
		int chessY[2][4]; // 两方棋子Y坐标 [颜色][棋子ID]
		
		bool canQueenMove(int x1, int y1, int x2, int y2, int x0 = -1, int y0 = -1) const;
		
		ChessBoard();  // 构造函数：初始化初始布局
		explicit ChessBoard(std::istream &in);
		
		int getTurn() const { return turn; }
		int getColor() const { return color; }
		const int *operator[](int x) const { return grid[x]; }  // 重载[]访问格子
		const Action previousAction(int turn) const { return acts[turn]; }  // 获取历史动作
		int getChessX(int color, int id) const { return chessX[color][id]; }
		int getChessY(int color, int id) const { return chessY[color][id]; }
		int getWinner() const { return !color; }  // 获胜方（无法移动方输）
		
		void doAction(const Action &act);  // 执行动作
		void revert();  // 撤销动作（回溯用）
		void init();
		bool canMove(int x, int y) const;  // 判断(x,y)位置是否有可移动棋子
		bool isFinished() const;  // 是否结束
		bool actValid(const Action &act) const;  // 动作是否合法
		bool isEmptyAfterMove(int x, int y, const Action &act) const;  // 移动后是否空
		void save(std::ostream &out);
	};
	
// ==================== 评估类 ====================
	
	const unsigned int infUInt = (unsigned int)1e9;
	
	inline double pow_2(unsigned int d) {
		if (d > 63) return 0;  // 超过63位返回0
		return 1.0 / (1ull << d);  // 计算1/2^d
	}
	
	// 估值函数：基于BFS计算各方控制区域
	class EvalField {
	private:
		const ChessBoard &board;  // 引用棋盘
		unsigned int d[2][2][gridSize][gridSize];  // 距离数组[颜色][模式][x][y]
		std::pair<int, int> q[gridSize * gridSize];  // BFS队列
		
		// BFS计算color方棋子到各格子的距离
		void bfs(int color, int type);
		double delta(int n, int m) const;  // 比较两个距离
		
	public:
		explicit EvalField(const ChessBoard &board) : board(board) {
			// 初始化所有距离为无穷大
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
					for (int x = 0; x < gridSize; x++)
						for (int y = 0; y < gridSize; y++)
							d[i][j][x][y] = infUInt;
		}
		
		double evaluate();  // 评估棋盘局面
	};
	
// ==================== MCTS 树 ====================
	
	class MCTree {
	protected:
		int nodeCnt = 0;  // 节点计数
		ChessBoard board;  // 当前棋盘状态
		EvalField field;   // 估值器
		
		// ===== 随机模拟相关 =====
		// 随机选择一个皇后的移动
		const Action randMove() const;
		// 为给定移动随机选择射箭位置
		const Action randArrow(const Action &move) const;
		// 生成完整随机动作
		const Action randAction() const;
		// 模拟rollout直到结束或深度限制
		int rollout(int maxDepth);
		
		// ===== 节点扩展记忆化 =====
		// 记录扩展进度，避免重复生成合法动作
		struct ExpandMemory {
			short id, o, l;  // id:棋子编号, o:方向, l:步长
			explicit ExpandMemory(short id = 0, short o = 0, short l = 0) : id(id), o(o), l(l) {}
		};
		
		// ===== MCTS节点结构 =====
		struct Node {
			std::vector<Node*> son;  // 子节点
			Node *fa;                // 父节点
			Action act;              // 导致该状态的动作
			int visit = 0;           // 访问次数
			double rate = 0;         // 胜率估计
			ExpandMemory mem;        // 扩展记忆（仅用于尚未完全扩展的节点）
			
			Node(Node *fa, const Action &act) : fa(fa), act(act) {}
			
			// UCT公式计算节点质量：rate + C*sqrt(log(N)/n)
			double calcQuality() const;
			// 更新节点统计信息
			void update(int color, const std::pair<int, int> &ratio);
			
			~Node() {
				for (auto i : son) delete i;  // 递归删除子树
			}
		} *root;  // 根节点
		
		// ===== 核心MCTS操作 =====
		Node *newNode(Node *fa = nullptr, const Action &act = Action());
		// 扩展节点（生成一个未访问的子节点）
		Node *expand(Node *k);
		// 选择路径并执行模拟
		const std::pair<int, int> choose(Node *k);
		
	public:
		explicit MCTree(const ChessBoard &board = ChessBoard());
		~MCTree();
		
		int getNodeCnt() const { return nodeCnt; }
		// 主入口：在限定时间内获取最佳动作
		const Action getAction(int ms);
		void doAction(const Action &act);  // 执行动作并更新树
		void revert();  // 撤销动作
		const ChessBoard &getBoard() const { return board; }
	};
	
// ==================== Bot 类 ====================
	
	// 对外接口封装
	class Bot {
	private:
		MCTree *tree;
	public:
		explicit Bot(const ChessBoard &board = ChessBoard());
		~Bot() { delete tree; }
		
		void doAction(const Action &act);
		const Action getAction(double sec);  // sec:秒数
		const ChessBoard &getBoard() const { return tree->getBoard(); }
	};
	
// ==================== 实现部分 ====================
	
// ChessBoard 实现
	ChessBoard::ChessBoard() : turn(0), color(Black) {
		for (int i = 0; i < gridSize; i++)
			for (int j = 0; j < gridSize; j++)
				grid[i][j] = Empty;
		
		// 初始布局（标准亚马逊棋摆法）
		grid[0][2] = Black; grid[0][5] = Black;
		grid[2][0] = Black; grid[2][7] = Black;
		grid[5][0] = White; grid[5][7] = White;
		grid[7][2] = White; grid[7][5] = White;
		
		// 记录初始棋子位置
		chessX[Black][0] = 0; chessY[Black][0] = 2;
		chessX[Black][1] = 0; chessY[Black][1] = 5;
		chessX[Black][2] = 2; chessY[Black][2] = 0;
		chessX[Black][3] = 2; chessY[Black][3] = 7;
		
		chessX[White][0] = 5; chessY[White][0] = 0;
		chessX[White][1] = 5; chessY[White][1] = 7;
		chessX[White][2] = 7; chessY[White][2] = 2;
		chessX[White][3] = 7; chessY[White][3] = 5;
	}
	
	// 执行动作：移动皇后+射箭
	void ChessBoard::doAction(const Action &act) {
		grid[act.x0][act.y0] = Empty;      // 原位置变空
		grid[act.x1][act.y1] = color;      // 目标位置放置皇后
		grid[act.x2][act.y2] = Arrow;      // 射箭位置变障碍
		
		// 更新被移动皇后的坐标记录
		for (int id = 0; id < 4; ++id) {
			if (chessX[color][id] == act.x0 && chessY[color][id] == act.y0) {
				chessX[color][id] = act.x1;
				chessY[color][id] = act.y1;
				break;
			}
		}
		
		acts[++turn] = act;  // 记录动作
		color ^= 1;          // 切换玩家
	}
	
	// 撤销动作（用于rollback）
	void ChessBoard::revert() {
		Action act = acts[turn--];
		color ^= 1;
		
		grid[act.x2][act.y2] = Empty;  // 移除箭
		grid[act.x1][act.y1] = Empty;  // 移除皇后
		grid[act.x0][act.y0] = color;  // 恢复原皇后
		
		// 恢复棋子坐标记录
		for (int id = 0; id < 4; ++id) {
			if (chessX[color][id] == act.x1 && chessY[color][id] == act.y1) {
				chessX[color][id] = act.x0;
				chessY[color][id] = act.y0;
				break;
			}
		}
	}
	
	// 判断(x,y)位置的皇后是否有合法移动
	bool ChessBoard::canMove(int x, int y) const {
		for (int i = 0; i < 8; ++i) {
			int tx = x + dx[i];
			int ty = y + dy[i];
			if (coordValid(tx, ty) && grid[tx][ty] == Empty) return true;
		}
		return false;
	}
	
	// 游戏是否结束（当前玩家无合法移动）
	bool ChessBoard::isFinished() const {
		for (int id = 0; id < 4; ++id) {
			if (canMove(chessX[color][id], chessY[color][id])) return false;
		}
		return true;
	}
	
	// 判断皇后是否可以从(x1,y1)移动到(x2,y2)
	// x0,y0是例外位置（用于射箭时忽略被移动的皇后）
	bool ChessBoard::canQueenMove(int x1, int y1, int x2, int y2, int x0, int y0) const {
		for (int o = 0; o < 8; ++o) {
			int x = x1 + dx[o], y = y1 + dy[o];
			while (coordValid(x, y)) {
				if (x == x2 && y == y2) return true;  // 找到目标
				// 遇到障碍停止（排除例外位置）
				if (!(x == x0 && y == y0) && grid[x][y] != Empty) break;
				x += dx[o];
				y += dy[o];
			}
		}
		return false;
	}
	
	// 判断动作是否合法
	bool ChessBoard::actValid(const Action &act) const {
		if (grid[act.x0][act.y0] != color) return false;
		if (grid[act.x1][act.y1] != Empty) return false;
		if (!(act.x0 == act.x2 && act.y0 == act.y2) && grid[act.x2][act.y2] != Empty) return false;
		// 检查移动和射箭路径
		return canQueenMove(act.x0, act.y0, act.x1, act.y1) &&
		canQueenMove(act.x1, act.y1, act.x2, act.y2, act.x0, act.y0);
	}
	
	// 判断执行动作act后，(x,y)位置是否为空
	bool ChessBoard::isEmptyAfterMove(int x, int y, const Action &act) const {
		if (x == act.x0 && y == act.y0) return true;  // 原位置变空
		if (x == act.x1 && y == act.y1) return false; // 目标位置被占
		return grid[x][y] == Empty;  // 其他位置看原状态
	}
	
// EvalField 实现
	// BFS计算color方所有棋子到各格子的距离
	void EvalField::bfs(int color, int type) {
		for (int x = 0; x < gridSize; ++x)
			for (int y = 0; y < gridSize; ++y)
				d[color][type][x][y] = infUInt;
		
		int l = 0, r = -1;
		// 将所有棋子作为BFS起点
		for (int id = 0; id < 4; ++id) {
			int x = board.getChessX(color, id);
			int y = board.getChessY(color, id);
			d[color][type][x][y] = 0;
			q[++r] = std::make_pair(x, y);
		}
		
		while (l <= r) {
			auto [x, y] = q[l++];
			for (int o = 0; o < 8; ++o) {
				int tx = x + dx[o], ty = y + dy[o];
				while (coordValid(tx, ty) && board[tx][ty] == Empty) {
					if (d[color][type][tx][ty] > d[color][type][x][y] + 1) {
						d[color][type][tx][ty] = d[color][type][x][y] + 1;
						q[++r] = std::make_pair(tx, ty);
					}
					tx += dx[o];
					ty += dy[o];
					if (!type) break;  // type=0:单步，type=1:无限延伸
				}
			}
		}
	}
	
	// 比较两个距离，返回评估值
	double EvalField::delta(int n, int m) const {
		if (n == infUInt && m == infUInt) return 0;  // 都不可达
		if (n == m) return 0.1;  // 距离相等，微小优势
		if (n < m) return 1;     // n更近，优势
		return -1;               // m更近，劣势
	}
	
	// 主评估函数：计算棋盘局面价值
	double EvalField::evaluate() {
		bfs(0, 1);  // 黑方可达距离
		bfs(1, 1);  // 白方可达距离
		
		double t1 = 0, c1 = 0;
		
		// 对每个空格评估：
		// 1. 比较双方最近棋子距离（delta）
		// 2. 计算区域控制优势（c1），距离越近权重越大
		for (int x = 0; x < gridSize; ++x) {
			for (int y = 0; y < gridSize; ++y) {
				if (board[x][y] == Empty) {
					t1 += delta(d[board.getColor()][1][x][y], d[!board.getColor()][1][x][y]);
					c1 += 2 * (pow_2(d[board.getColor()][1][x][y]) - pow_2(d[!board.getColor()][1][x][y]));
				}
			}
		}
		
		t1 -= 0.5;  // 微小偏置
		// 前40回合混合两种评估，后期只用t1
		if (board.getTurn() < 40)
			return t1 * (board.getTurn() * 1.0 / 40) + c1 * (1 - (board.getTurn() * 1.0 / 40));
		return t1;
	}
	
// MCTree 实现
	MCTree::MCTree(const ChessBoard &board) : board(board), field(this->board), root(newNode()) {
		// 重建树到当前回合
		for (int i = 1; i <= board.getTurn(); ++i)
			doAction(board.previousAction(i));
	}
	
	MCTree::~MCTree() {
		delete root;
	}
	
	MCTree::Node* MCTree::newNode(Node *fa, const Action &act) {
		++nodeCnt;
		Node *ch = new Node(fa, act);
		if (fa) fa->son.push_back(ch);
		return ch;
	}
	
	// 随机选择一个合法移动（不包含射箭）
	const Action MCTree::randMove() const {
		int cnt = 0;
		// 统计所有合法移动总数
		for (int id = 0; id < 4; ++id) {
			int x = board.getChessX(board.getColor(), id);
			int y = board.getChessY(board.getColor(), id);
			for (int o = 0; o < 8; ++o) {
				int tx = x + dx[o], ty = y + dy[o];
				while (coordValid(tx, ty) && board[tx][ty] == Empty) {
					++cnt;
					tx += dx[o];
					ty += dy[o];
				}
			}
		}
		
		// 随机选择第t个移动
		int t = rand() % cnt + 1;
		for (int id = 0; id < 4; ++id) {
			int x = board.getChessX(board.getColor(), id);
			int y = board.getChessY(board.getColor(), id);
			for (int o = 0; o < 8; ++o) {
				int tx = x + dx[o], ty = y + dy[o];
				while (coordValid(tx, ty) && board[tx][ty] == Empty) {
					if (--t == 0) return Action(x, y, tx, ty, 0, 0);
					tx += dx[o];
					ty += dy[o];
				}
			}
		}
		return Action();  // 不应该到达这里
	}
	
	// 为给定移动随机选择射箭位置
	const Action MCTree::randArrow(const Action &move) const {
		int cnt = 0;
		int x = move.x1, y = move.y1;
		// 统计从目标位置可射的所有位置
		for (int o = 0; o < 8; ++o) {
			int tx = x + dx[o], ty = y + dy[o];
			while (coordValid(tx, ty) && board.isEmptyAfterMove(tx, ty, move)) {
				++cnt;
				tx += dx[o];
				ty += dy[o];
			}
		}
		
		int t = rand() % cnt + 1;
		for (int o = 0; o < 8; ++o) {
			int tx = x + dx[o], ty = y + dy[o];
			while (coordValid(tx, ty) && board.isEmptyAfterMove(tx, ty, move)) {
				if (--t == 0) return Action(move.x0, move.y0, move.x1, move.y1, tx, ty);
				tx += dx[o];
				ty += dy[o];
			}
		}
		return Action();
	}
	
	// 生成完整随机动作
	const Action MCTree::randAction() const {
		return randArrow(randMove());
	}
	
	// rollout模拟：随机走子直到结束或深度限制
	int MCTree::rollout(int maxDepth) {
		int depth = 0;
		while (!board.isFinished() && depth < maxDepth) {
			++depth;
			board.doAction(randAction());  // 随机动作
		}
		
		int win = -1;
		if (board.isFinished()) {
			win = board.getWinner();  // 游戏结束，返回获胜方
		} else {
			// 达到深度限制，使用估值函数判断
			double eval = field.evaluate();
			if (eval > 0.1) win = board.getColor();  // 当前方优势
			else if (eval < -0.1) win = !board.getColor();  // 对方优势
		}
		
		while (depth--) board.revert();  // 撤销所有模拟动作
		return win;
	}
	
	// 扩展节点：生成一个未访问过的子节点
	MCTree::Node* MCTree::expand(Node *k) {
		// 所有可能性已枚举完
		if (k->mem.id == 4 || k->mem.o == 8) return nullptr;
		
		// 如果当前节点是移动部分，扩展射箭部分
		if (k->act.x0 == -1) {
			int x = board.getChessX(board.getColor(), k->mem.id);
			int y = board.getChessY(board.getColor(), k->mem.id);
			++k->mem.l;  // 增加步长
			int tx = x + k->mem.l * dx[k->mem.o];
			int ty = y + k->mem.l * dy[k->mem.o];
			
			// 跳过无效位置
			while (!(coordValid(tx, ty) && board[tx][ty] == Empty)) {
				k->mem.l = 1;
				if (k->mem.o < 7) ++k->mem.o;  // 下一个方向
				else ++k->mem.id, k->mem.o = 0; // 下一个棋子
				if (k->mem.id == 4) return nullptr;  // 所有棋子枚举完
				
				x = board.getChessX(board.getColor(), k->mem.id);
				y = board.getChessY(board.getColor(), k->mem.id);
				tx = x + k->mem.l * dx[k->mem.o];
				ty = y + k->mem.l * dy[k->mem.o];
			}
			return newNode(k, Action(x, y, tx, ty, -1, -1));
		} else {
			// 扩展射箭动作
			int x = k->act.x1, y = k->act.y1;
			++k->mem.l;
			int tx = x + k->mem.l * dx[k->mem.o];
			int ty = y + k->mem.l * dy[k->mem.o];
			
			// 跳过无效位置
			while (!(coordValid(tx, ty) && board.isEmptyAfterMove(tx, ty, k->act))) {
				k->mem.l = 1;
				++k->mem.o;
				if (k->mem.o == 8) return nullptr;  // 所有方向枚举完
				tx = x + k->mem.l * dx[k->mem.o];
				ty = y + k->mem.l * dy[k->mem.o];
			}
			return newNode(k, Action(-1, -1, x, y, tx, ty));
		}
	}
	
	// MCTS核心：选择-扩展-模拟-回溯
	const std::pair<int, int> MCTree::choose(Node *k) {
		std::pair<int, int> ratio = std::make_pair(0, 0);  // 统计胜负比（黑胜，白胜）
		Node *ch = expand(k);  // 尝试扩展
		
		if (ch) {
			// ===== 扩展成功：模拟新节点 =====
			if (k->act.x0 != -1) {
				// 父节点是移动部分，执行完整动作后模拟
				board.doAction(k->act * ch->act);
				for (int i = 5; i <= 6; ++i) {  // 模拟2次（不同深度）
					switch (rollout(i)) {
						case Black: ratio.first += 1; break;  // 黑方胜
						case White: ratio.second += 1; break; // 白方胜
						default: break;  // 平局
					}
				}
				board.revert();
				ch->update(board.getColor(), ratio);  // 更新子节点
				k->update(board.getColor(), ratio);   // 更新父节点
			} else {
				// 父节点是根，只模拟移动部分
				for (int i = 4; i <= 5; ++i) {
					switch (rollout(i)) {
						case Black: ratio.first += 1; break;
						case White: ratio.second += 1; break;
						default: break;
					}
				}
				k->update(!board.getColor(), ratio);  // 注意颜色切换
			}
		} else {
			// ===== 扩展失败：选择最佳子节点 =====
			for (auto s : k->son) {
				if (!ch || s->calcQuality() > ch->calcQuality()) {
					ch = s;  // UCT选择
				}
			}
			
			if (!ch) {
				// 无合法动作，当前方输
				if (board.getColor() == Black) ratio = std::make_pair(0, 2);
				else ratio = std::make_pair(2, 0);
				k->update(board.getColor(), ratio);
			} else {
				// 递归处理子节点
				if (ch->act.x2 != -1) {
					// 子节点是完整动作，执行后递归
					board.doAction(k->act * ch->act);
					ratio = choose(ch);
					board.revert();
					k->update(board.getColor(), ratio);
				} else {
					// 子节点是移动部分，直接递归
					ratio = choose(ch);
					k->update(!board.getColor(), ratio);
				}
			}
		}
		return ratio;
	}
	
	// 主入口：在clocks个时钟周期内搜索最佳动作
	const Action MCTree::getAction(int ms) {
			using namespace std::chrono;
			auto start = steady_clock::now();
		while (duration_cast<milliseconds>(steady_clock::now() - start).count() < ms) {
			choose(root);  // 执行一次完整MCTS迭代
		}
		
		// 选择访问次数最多的完整动作
		Node *arrow = nullptr;
		for (auto move : root->son) {
			for (auto s : move->son) {
				if (!arrow || s->visit > arrow->visit) {
					arrow = s;
				}
			}
		}
		
		return arrow ? arrow->fa->act * arrow->act : Action();
	}
	
	// 执行动作并更新树结构
	void MCTree::doAction(const Action &act) {
		Action move = act.getMove();  // 移动部分
		Action arrow = act.getArrow(); // 射箭部分
		
		// 在子节点中查找匹配，若找到则提升为根
		Node *ch = nullptr;
		for (auto s : root->son) if (s->act == move) ch = s;
		if (ch) root = ch;
		else root = newNode(root, move);  // 未找到则创建
		
		ch = nullptr;
		for (auto s : root->son) if (s->act == arrow) ch = s;
		if (ch) root = ch;
		else root = newNode(root, arrow);
		
		board.doAction(act);  // 更新棋盘
	}
	
	// 撤销动作（用于调试和分析）
	void MCTree::revert() {
		board.revert();
		root = root->fa->fa;  // 回到上上个节点
	}
	
	// UCT公式：rate + 0.5*sqrt(log(N)/n)
	double MCTree::Node::calcQuality() const {
		if (!visit) return 10;  // 未访问节点优先探索
		return 1.0 * rate + 0.5 * sqrt(log(fa->visit) / visit);
	}
	
	// 更新节点统计：加权平均胜率
	void MCTree::Node::update(int color, const std::pair<int, int> &ratio) {
		int p = 0, q = (ratio.first + ratio.second);  // 总对局数
		if (color == Black) p = ratio.first;  // 当前方获胜数
		else p = ratio.second;
		
		// 增量更新胜率估计：避免除零，使用加权平均
		double c = 1.0 - 1.0 / (visit + 500);
		rate += (p - q * rate) / (c * visit + q);
		visit += ratio.first + ratio.second;
	}
	
// Bot 实现
	Bot::Bot(const ChessBoard &board) : tree(new MCTree(board)) {}
	
	void Bot::doAction(const Action &act) {
		tree->doAction(act);
	}
	
	// 获取动作（sec:秒数）
	const Action Bot::getAction(double sec) {
			return tree->getAction(int(sec * 990));
	}
	
} // namespace AmazonGame

// ===== 外部接口适配层 =====

// 将外部棋盘表示转换为内部表示并调用MCTS
Movement getBestMove3(const std::array<std::array<int, n>, n>& board, const bool is_black) {
	using namespace AmazonGame;
	
	// 创建棋盘并转换表示
	ChessBoard cb;
	
	// 清空棋盘
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			cb.grid[i][j] = Empty;
		}
	}
	
	// 转换棋子表示 (0=空地, 1=黑, -1=白, 2=障碍) -> (-1=空地, 0=黑, 1=白, 2=障碍)
	int pieceCount[2] = {0, 0};
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			if (board[i][j] == 1) { // 黑棋
				if (pieceCount[Black] < 4) {
					cb.chessX[Black][pieceCount[Black]] = i;
					cb.chessY[Black][pieceCount[Black]] = j;
					cb.grid[i][j] = Black;
					pieceCount[Black]++;
				}
			} else if (board[i][j] == -1) { // 白棋
				if (pieceCount[White] < 4) {
					cb.chessX[White][pieceCount[White]] = i;
					cb.chessY[White][pieceCount[White]] = j;
					cb.grid[i][j] = White;
					pieceCount[White]++;
				}
			} else if (board[i][j] == 2) { // 障碍
				cb.grid[i][j] = Arrow;
			}
		}
	}
	
	// 设置当前玩家
	cb.color = is_black ? Black : White;
	
	// 创建Bot并获取最佳走法 (1秒思考时间)
	Bot bot(cb);
	Action act = bot.getAction(1.0);
	
	// 转换为Movement格式 [x0, y0, x1, y1, x2, y2]
	if (act.isEmpty()) {
		return {-1, -1, -1, -1, -1, -1};
	}
	return {act.x0, act.y0, act.x1, act.y1, act.x2, act.y2};
}
