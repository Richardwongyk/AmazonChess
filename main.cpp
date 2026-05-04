/**
 * 亚马逊棋游戏主程序
 * 作者：王昱焜
 * 描述：基于EGE图形库的亚马逊棋游戏
 */

#include "game.h"
#include "platform/EGEPlatform.h"
#include <iostream>

int main() {
    try {
        EGEPlatform platform;
        Game amazons_game(&platform);

        if (!amazons_game.initialize()) {
            std::cerr << "游戏初始化失败！" << std::endl;
            return 1;
        }

        amazons_game.run();

    } catch (const std::exception& e) {
        std::cerr << "程序运行出错：" << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "未知错误发生！" << std::endl;
        return 1;
    }

    return 0;
}
