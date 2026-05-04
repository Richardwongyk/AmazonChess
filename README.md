# Amazons（亚马逊棋）

基于 C++17 和 EGE 图形库的亚马逊棋（Game of the Amazons）桌面游戏。支持人人对战、人机对战（三级 AI 难度），具备完整的图形界面和存档系统。

> 📦 **只想玩？** 跳转到 [快速上手](#快速上手) 直接下载运行。

---

## 目录

- [游戏规则](#游戏规则)
- [功能特性](#功能特性)
- [快速上手](#快速上手)
- [从源码编译](#从源码编译)
- [操作说明](#操作说明)
- [AI 系统](#ai-系统)
- [项目结构](#项目结构)
- [跨平台设计](#跨平台设计)
- [常见问题](#常见问题)

---

## 游戏规则

### 简介

**亚马逊棋**（Game of the Amazons，又称 Amazon Chess）是 1988 年由阿根廷人 Walter Zamkauskas 发明的抽象策略棋类游戏。它融合了国际象棋皇后的走法和围棋的围地思想，规则简单但策略深度极高。

### 棋盘与棋子

棋盘为 **8×8** 的方格。双方各执 **4 枚皇后**：

```
   0   1   2   3   4   5   6   7         ← 列号
0  ·   ·   ○   ·   ·   ●   ·   ·        ○ = 白方皇后
1  ·   ·   ·   ·   ·   ·   ·   ·        ● = 黑方皇后
2  ○   ·   ·   ·   ·   ·   ·   ○
3  ·   ·   ·   ·   ·   ·   ·   ·
4  ·   ·   ·   ·   ·   ·   ·   ·
5  ●   ·   ·   ·   ·   ·   ·   ●
6  ·   ·   ·   ·   ·   ·   ·   ·
7  ·   ·   ○   ·   ·   ●   ·   ·
    ↑ 行号
```

黑方初始位置：(0,2) (2,0) (5,0) (7,2)
白方初始位置：(0,5) (2,7) (5,7) (7,5)

### 回合流程

每回合分 **两步**，缺一不可：

**第一步：移动己方皇后**

选择你的一枚皇后，像国际象棋的皇后一样沿 **八个方向**（水平、垂直、对角线）移动任意格数。移动路径上不能有其他棋子或障碍物，目标格必须为空。

```
示例：皇后沿对角线移动 3 格
  ·  ·  ·  ·          ·  ·  ·  ·
  ·  ●  ·  ·    →     ·  ·  ·  ·
  ·  ·  ·  ·          ·  ·  ●  ·      ↙ 只能走直线
  ·  ·  ·  ·          ·  ·  ·  ·
```

**第二步：射出障碍箭**

从皇后移动后的新位置，沿八个方向之一射出障碍箭。箭的走法与皇后相同——沿直线飞行任意格数，路径和目标格必须为空。箭永久占据目标格，此后双方均不能通过或停留。

```
示例：皇后到达新位置后，向上射出障碍箭
  ·  ·  ✖  ·          ← 箭落点（永久障碍）
  ·  ·  ·  ·
  ·  ·  ●  ·          ← 皇后新位置
  ·  ·  ·  ·
```

### 胜负判定

当一方的所有皇后都无法移动时（所有可达格均被棋子或障碍占据），该方**落败**，对方获胜。

> 💡 核心策略：在保证自己皇后可移动的前提下，用障碍箭封锁对手皇后的活动空间。

### 走法示例（完整一回合）

```
黑方的回合：
  开始前              移动皇后后           射出障碍后
  ·  ●₁ ·  ·          ·  ·  ·  ·          ·  ·  ·  ·
  ·  ·  ·  ·          ·  ·  ·  ·          ·  ·  ·  ·
  ·  ·  ·  ·    →     ·  ·  ●₁ ·    →     ·  ·  ●₁ ·
  ·  ·  ·  ·          ·  ·  ·  ·          ·  ✖  ·  ·
                                         ↑ 箭落此处，永久占用
```

### 官方变体说明

标准的亚马逊棋使用 **10×10** 棋盘，每方 4 枚皇后。本项目采用 **8×8** 棋盘变体，节奏更快，适合入门和快速对局。

---

## 功能特性

### 游戏模式

| 模式 | 说明 |
|------|------|
| 人人对战 | 两人共用一台电脑，轮流点击操作 |
| 人机对战（执黑） | 你先手，AI 后手 |
| 人机对战（执白） | AI 先手，你后手 |

### AI 难度（详见 [AI 系统](#ai-系统)）

| 难度 | 算法 | 适合 |
|------|------|------|
| 简单 | 随机走法 | 新手入门，轻松获胜 |
| 中等 | Minimax + 评估函数 | 有一定挑战 |
| 困难 | 蒙特卡洛树搜索（MCTS） | 高手对决，约 1 秒思考时间 |

### 存档系统

- 支持保存 **最多 100 条** 对局记录
- 存档包含完整棋盘状态、当前回合、操作阶段（选子/移动/放箭）、游戏模式
- 支持读取、覆盖（同对局自动覆盖旧档）、单条删除、一键清空
- 存档文件为 `amazons_saves.dat`（二进制格式），存储在程序运行目录

### 图形界面

- 棋盘格双色渲染（浅棕 / 深棕交替）
- 棋子带立体光影效果（高光 + 阴影）
- 可移动位置绿色高亮，可放箭位置青色高亮
- 选中棋子黄色圆环标记
- 游戏结束半透明遮罩 + 对话框
- 窗口标题和图标

---

## 快速上手

### 直接运行（无需编译）

1. 前往 [Releases](https://github.com/Richardwongyk/AmazonChess/releases) 下载最新版 `Amazons.zip`
2. 解压到任意目录
3. 确保 `Amazons-local.exe` 和 `music.wav` 在同一文件夹
4. 双击 `Amazons-local.exe` 即可运行

> 🖥️ 仅支持 **64 位 Windows**（7 / 10 / 11）。不需要安装任何运行时或 DLL。
>
> 🎵 `music.wav` 缺失不影响游戏功能，只是没有背景音乐。

---

## 从源码编译

### 环境要求

- **编译器**：MinGW-w64 GCC 8.x ~ 15.x（`g++.exe` 需在 PATH 中）
- **构建工具**：`mingw32-make.exe`（MinGW 自带）或 CMake 3.15+
- **操作系统**：Windows（EGE 图形库依赖 Windows GDI+）

> ❓ 没有 MinGW？推荐下载 [winlibs MinGW-w64](https://winlibs.com/) 或 [MSYS2](https://www.msys2.org/)，解压后将 `bin/` 目录加入系统 PATH。

### 使用 Make（推荐）

```bash
git clone https://github.com/Richardwongyk/AmazonChess.git
cd AmazonChess
mingw32-make
./Amazons-local.exe
```

### 使用 CMake

```bash
git clone https://github.com/Richardwongyk/AmazonChess.git
cd AmazonChess
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
../Amazons-local.exe
```

### 编译选项

Makefile 中的编译选项与 CMakeLists.txt 保持一致：

```
C++ 标准:   gnu++17
优化级别:   -O2
链接方式:   静态链接 (-static)，无外部 DLL 依赖
堆栈大小:   12 MB（AI 递归搜索需要较大栈空间）
```

---

## 操作说明

### 主菜单

| 按键 | 功能 |
|------|------|
| `A` | 开始新游戏 → 进入模式选择 |
| `B` | 载入存档 → 进入存档列表 |
| `C` 或 `ESC` | 退出游戏 |

### 模式选择

| 按键 | 模式 |
|------|------|
| `1` | 人人对战（两人在同一台电脑上交替操作） |
| `2` | 人机对战 —— 你执黑先手 |
| `3` | 人机对战 —— 你执白后手 |
| `0` | 返回主菜单 |

### 难度选择（仅人机模式）

| 按键 | 难度 | 说明 |
|------|------|------|
| `A` | 简单 | AI 随机走子，适合新手 |
| `B` | 中等 | AI 使用 Minimax 搜索，有一定棋力 |
| `C` | 困难 | AI 使用 MCTS 搜索，思考约 1 秒 |
| `Q` | 返回模式选择 |

### 对局中

| 操作 | 说明 |
|------|------|
| 鼠标左键（第 1 次） | **选择棋子**：点击己方皇后，可移动位置高亮为绿色 |
| 鼠标左键（第 2 次） | **移动棋子**：点击绿色高亮格，皇后移动过去 |
| 鼠标左键（第 3 次） | **放置障碍**：点击青色高亮格，障碍箭落下，回合结束 |
| `S` | **保存并退出**：保存当前对局到存档，返回主菜单 |
| `0` | **不保存退出**：放弃当前对局，返回主菜单 |
| `ESC` | **退出游戏**：关闭窗口 |

> 💡 在选择棋子阶段也可以重新点击其他己方棋子来切换选中。
>
> 💡 在移动阶段也可以点击其他己方棋子来切换要移动的皇后。

### 存档界面

| 按键 | 说明 |
|------|------|
| `1` ~ `5` | 载入对应编号的存档 |
| `Delete` | 清空全部存档记录 |
| `0` | 返回主菜单 |
| `ESC` | 退出游戏 |

存档列表按照保存时间倒序排列（最新存档在最前面），最多显示 5 条，最多存储 100 条。

---

## AI 系统

AI 引擎位于 `AI_Kernel.cpp`，与图形界面完全解耦，可独立使用。

### 简单：随机算法

遍历当前玩家所有合法走法（皇后移动 × 障碍箭落点），从中**均匀随机**选择一个。没有任何策略性，纯粹随机。

### 中等：Minimax 搜索

- **搜索深度**：2 层（自己走一步 → 对手走一步）
- **评估函数**：综合计算活动性（皇后可走格数）、领地控制（BFS 独占区域）、中心优势、对手封锁奖励
- **候选剪枝**：先评出前 100 个最佳走法，仅对前 30 个做深度搜索
- **性能**：< 0.1 秒

### 困难：蒙特卡洛树搜索（MCTS）

- **选择**：UCT 公式平衡探索与利用
- **扩展**：逐方向逐棋子枚举走法
- **模拟**：随机 Rollout 至游戏结束或深度限制
- **回溯**：更新胜率估计
- **限时**：约 1 秒思考时间（可调整 `getBestMove3` 中的参数）
- **节点数**：每秒约数千次 MCTS 迭代

### 棋盘评估函数（BFS）

MCTS 使用的评估函数基于广度优先搜索（BFS），计算双方皇后的可达区域：

- 对每个空格，比较黑方和白方谁更先到达
- 晚期局面以"更近即优势"为主，早期局面混合区域控制权重
- 前 40 回合权重渐进过渡

---

## 项目结构

```
AmazonChess/
├── main.cpp                      # 程序入口，创建平台和游戏对象
├── Game.cpp / Game.h             # 游戏主控：状态机、输入分发、回合管理
├── Game_board.cpp / Game_board.h # 棋盘逻辑：走法生成、合法性检查、终局判定
├── AI_Kernel.cpp / AI_Kernel.h   # AI 引擎：随机 / Minimax / MCTS
├── Renderer.cpp / Renderer.h     # 图形渲染：棋盘、棋子、UI、特效
├── SaveManager.cpp / SaveManager.h # 存档系统：二进制读写、增删改查
├── Constants.h                   # 常量定义：棋盘尺寸、初始位置、枚举类型
│
├── platform/                     # 🔌 平台抽象层
│   ├── IPlatform.h               #   抽象接口：窗口/绘图/输入/音频 (~25 个方法)
│   ├── EGEPlatform.h / .cpp      #   EGE 实现 (Windows GDI+)
│   └── KeyCode.h                 #   跨平台键码定义
│
├── lib/                          # 📚 第三方依赖
│   ├── libgraphics.a             #   EGE 图形库 v25.11（静态库，1.2 MB）
│   └── include/                  #   EGE 头文件（9 个）
│
├── Amazons-local_private.h / .rc # Windows 资源文件（版本信息 + 图标）
├── app.ico                       # 应用程序图标
├── music.wav                     # 背景音乐 (~78 MB)
├── Makefile                      # GNU Make 构建文件
├── CMakeLists.txt                # CMake 构建文件
├── .gitignore                    # Git 忽略规则
└── README.md                     # 本文件
```

### 模块依赖关系

```
main.cpp
  └─ Game
       ├─ GameBoard ─── Constants
       ├─ Renderer ──── Constants, SaveManager
       │    └─ IPlatform ← EGEPlatform (EGE / Windows)
       ├─ AI_Kernel ─── (纯计算，无依赖)
       └─ SaveManager ─ Constants
```

- **平台无关层**（可直接在其他 OS 编译）：`Game_board`、`AI_Kernel`、`SaveManager`、`Constants`
- **平台相关层**（需移植）：`EGEPlatform`、`Renderer`（通过 `IPlatform` 接口调用平台功能）
- **胶水层**：`Game`（通过 `IPlatform` 调用输入/音频/计时）

---

## 跨平台设计

### 设计理念

项目通过 **平台抽象层**（`platform/IPlatform.h`）将操作系统相关的图形、输入、音频调用隔离在一个接口之后。

### 当前状态

| 层次 | 平台依赖 | 移植难度 |
|------|----------|----------|
| `Game_board` / `AI_Kernel` / `SaveManager` | 无 | 零（纯标准 C++） |
| `Game` | 仅通过 `IPlatform*` 指针 | 无需修改 |
| `Renderer` | 仅通过 `IPlatform*` 指针 | 无需修改 |
| `EGEPlatform` | EGE / Windows GDI+ | **唯一需要替换的文件** |

### 移植到其他平台

以 **SDL2** 为例，只需 3 步：

1. 编写 `SDLPlatform.h/.cpp`，实现 `IPlatform` 的全部虚函数
2. 修改 `main.cpp` 第 13 行：`EGEPlatform` → `SDLPlatform`
3. 在 Makefile 中把 `-lgraphics` 替换为 `-lSDL2`

> 类似的，移植到 SFML、raylib、Qt 等图形库只需替换这一个实现文件。游戏逻辑（棋盘、AI、存档）一行都不用改。

### IPlatform 接口一览

| 类别 | 方法 |
|------|------|
| 窗口 | `initWindow` `closeWindow` `isRunning` |
| 绘图 | `clearDevice` `flushWindow` `fillRect` `alphaFillRect` `drawRect` `fillCircle` `drawCircle` `drawLine` `fillEllipse` `drawText` `textWidth` |
| 样式 | `setFillColor` `setLineColor` `setBgColor` `setFont` `setLineStyle` `setTextBgMode` `enableAntiAlias` |
| 输入 | `isKeyDown` `hasMouseMsg` `getMouseMsg` |
| 音频 | `openMusic` `playMusic` `pauseMusic` `stopMusic` `closeMusic` |
| 计时 | `delayFPS` |

---

## 常见问题

### Q: 双击 exe 没反应？

- 确保 `music.wav` 和 exe 在同一目录下
- 尝试右键 →"以管理员身份运行"
- 检查杀毒软件是否拦截（本程序为静态编译，某些杀软可能误报）

### Q: 编译报错 "graphics.h not found"？

`lib/include/` 目录需在编译器的头文件搜索路径中。Makefile 已配置 `-Ilib/include`，不要删除该目录。

### Q: 音乐不播放？

- `music.wav` 须与 exe 在同一目录
- 仅支持 WAV 格式
- 文件较大（78MB），请确保完整下载

### Q: 存档在哪里？

存档文件 `amazons_saves.dat` 自动生成在程序运行目录下（通常与 exe 同目录）。

### Q: 能在 Linux / macOS 上玩吗？

目前不能。EGE 图形库依赖 Windows GDI+。但项目已设计好跨平台抽象层，未来可通过实现 SDL2 后端的 `IPlatform` 来支持。

### Q: 为什么用 8×8 棋盘而非标准的 10×10？

8×8 棋盘使对局节奏更快（约 15-30 回合），适合快速对弈和课程展示。改为 10×10 只需修改 `Constants.h` 中的 `N` 和初始位置数组。

---

## 技术栈

| 类别 | 技术 |
|------|------|
| 语言 | C++17 (gnu++17) |
| 编译器 | MinGW-w64 GCC 8.x ~ 15.x |
| 图形库 | [EGE v25.11](https://github.com/x-ege/xege) (Easy Graphics Engine) |
| 构建系统 | GNU Make + CMake 3.15+ |
| 目标平台 | Windows 7/10/11 x86_64 |

---

## 许可证

本项目为北京大学课程作业项目。EGE 图形库以 [MIT 许可证](https://github.com/x-ege/xege/blob/master/LICENSE) 开源。

---

## 作者

**王昱焜** — [GitHub @Richardwongyk](https://github.com/Richardwongyk)
