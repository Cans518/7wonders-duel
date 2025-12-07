#pragma once
#include <vector>
#include <memory>
#include "Board.h"

// 前向声明 (Forward Declaration)：
// 告诉编译器这些类存在，但在 .cpp 里才引用具体头文件。
// 这样可以减少头文件依赖冲突。
class Player;         // Member 3 的类
class CardStructure;  // Member 2 的类 (金字塔结构)

class Game {
private:
    static Game* instance; // 单例实例

    // 核心成员
    std::unique_ptr<Board> board;
    std::vector<std::shared_ptr<Player>> players; // 2名玩家
    
    // 游戏状态
    int currentAge;       // 当前时代 (1, 2, 3) 
    int currentPlayerIdx; // 当前回合玩家索引 (0 或 1)
    bool isGameOver;

    // 私有构造函数 (单例模式)
    Game(); 

public:
    // 获取单例
    static Game& getInstance();
    // 禁止拷贝
    Game(const Game&) = delete;
    void operator=(const Game&) = delete;

    // 初始化游戏
    // 规则参考: 
    void init();

    // 开始游戏主循环
    void run();

    // 处理回合逻辑
    // 规则参考: 
    void playTurn();

    // 检查是否有玩家达成 科技或军事 胜利
    // 规则参考: 
    bool checkSupremacyVictory();

    // 时代结束处理
    // 规则参考: 
    void endAge();
    
    // 辅助方法
    Board* getBoard() { return board.get(); }
    Player* getCurrentPlayer();
    Player* getOpponent();
};