# Amazons（亚马逊棋）

基于 C++17 和 EGE 图形库的亚马逊棋（Game of the Amazons）桌面游戏。支持人人对战、人机对战（三级AI难度），具备完整的存档读档系统。

---

## 游戏简介

**亚马逊棋** 是 1988 年由 Walter Zamkauskas 发明的抽象策略棋类。双方各执 4 枚皇后，在 8×8 的棋盘上对决。每回合分两步：**移动己方皇后**（如国际象棋皇后走法），然后**从新位置射出障碍箭**。障碍箭永久占据该格，双方均不可通行。率先无法移动的一方**落败**。

---

## 功能特性

- **双人对战**：两人共用一台电脑轮流操作
- **人机对战**：可选择执黑或执白，AI 提供三个难度档位
- **三级 AI**：
  - 简单 — 随机走法
  - 中等 — Minimax 搜索 + 评估函数（深度 2）
  - 困难 — 蒙特卡洛树搜索（MCTS），每秒约数千次模拟
- **完整存档系统**：保存/读取最多 100 条棋局记录，支持删除
- **图形界面**：棋盘高亮、选中效果、棋子立体渲染、半透明对话框
- **背景音乐**：WAV 格式循环播放

---

## 快速上手

### 直接运行（无需编译）

如果你用的是 **64 位 Windows**，解压后直接双击 `Amazons-local.exe` 即可。

> `music.wav` 须与 exe 放在同一目录下，否则没有背景音乐（不影响对局）。

### 从源码编译

**前提**：安装 MinGW-w64（任意版本均可，GCC 8 ~ 15 均测试通过），确保 `g++.exe` 和 `mingw32-make.exe` 在系统 PATH 中。

```bash
cd AmazonChess
mingw32-make
```

编译完成后运行：

```bash
./Amazons-local.exe
```

如果需要 CMake：

```bash
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
```

---

## 操作说明

### 菜单

| 按键 | 功能 |
|------|------|
| `A` | 新游戏 |
| `B` | 载入存档 |
| `C` / `ESC` | 退出游戏 |

### 游戏中

| 操作 | 说明 |
|------|------|
| 鼠标左键 | 选择棋子 → 移动棋子 → 放置障碍 |
| `S` | 保存当前对局并返回菜单 |
| `0` | 不保存直接返回菜单 |
| `ESC` | 退出游戏 |

### 模式选择

- `1` — 人人对战
- `2` — 人机对战（你执黑先手）
- `3` — 人机对战（你执白后手）

### 难度选择

- `A` — 简单
- `B` — 中等
- `C` — 困难

### 存档界面

- `1` ~ `5` — 载入对应存档
- `Delete` — 清空全部存档
- `0` — 返回菜单

---

## 项目结构

```
AmazonChess/
  main.cpp                  # 程序入口
  Game.cpp / Game.h         # 游戏主控逻辑
  Game_board.cpp / .h       # 棋盘状态与规则
  AI_Kernel.cpp / .h        # AI 引擎（随机/Minimax/MCTS）
  Renderer.cpp / .h         # 图形渲染
  SaveManager.cpp / .h      # 存档读写
  Constants.h               # 棋盘常量与枚举

  platform/                 # 平台抽象层
    IPlatform.h             #   抽象接口（窗口/绘图/输入/音频）
    EGEPlatform.h / .cpp    #   EGE 实现（Windows GDI+）
    KeyCode.h               #   跨平台键码定义

  lib/                      # EGE 图形库（第三方）
    libgraphics.a           #   EGE 静态库 v25.11
    include/                #   EGE 头文件

  Amazons-local_private.rc  # Windows 资源脚本（图标）
  app.ico                   # 程序图标
  music.wav                 # 背景音乐 (~78MB)
  Makefile / CMakeLists.txt # 构建文件
```

---

## 跨平台设计

项目通过 **平台抽象层**（`platform/IPlatform.h`）隔离了操作系统相关的调用。

- **当前实现**：`EGEPlatform` — 基于 EGE / Windows GDI+，仅支持 Windows
- **移植方式**：实现 `IPlatform` 接口（约 25 个方法）即可支持新平台

例如，要实现 SDL2 后端：
1. 编写 `SDLPlatform.h/.cpp`，实现 `IPlatform` 的每个方法
2. 修改 `main.cpp` 使用 `SDLPlatform` 替代 `EGEPlatform`
3. 链接 SDL2 库

游戏逻辑层（`Game_board`、`AI_Kernel`、`SaveManager`）**完全不依赖平台**，移植时无需修改。

---

## AI 技术细节

| 难度 | 算法 | 特点 |
|------|------|------|
| 简单 | 随机选择 | 从所有合法走法中均匀采样 |
| 中等 | Minimax (深度2) + 评估函数 | 评估活动性、领地控制、中心优势、封锁 |
| 困难 | MCTS（蒙特卡洛树搜索） | UCT 选择 + BFS 评估 + Rollout 模拟，每次限时约 1 秒 |

AI 引擎（`AI_Kernel`）与图形界面完全解耦，可直接用于 Botzone 等在线对战平台。

---

## 技术栈

- **语言**：C++17（gnu++17）
- **编译器**：MinGW-w64 GCC（8.x ~ 15.x 均兼容）
- **图形库**：[EGE (Easy Graphics Engine) v25.11](https://github.com/x-ege/xege)
- **构建**：GNU Make / CMake
- **平台**：Windows x86_64

---

## 许可证

本项目为北京大学课程作业项目。EGE 图形库以 MIT 许可证开源。

---

## 作者

王昱焜
