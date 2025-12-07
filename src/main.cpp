#include "core/Game.h"

int main() {
    // 启动游戏引擎
    Game& game = Game::getInstance();
    game.run();
    
    return 0;
}
