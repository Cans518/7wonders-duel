#pragma once
#include <vector>
#include <memory>
#include "Board.h"
#include "../player/game.h"
#include "../cards/member2.h"

// class Player;
// class CardStructure;

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
    std::unique_ptr<CardStructure> cardStructure;

    // 初始化游戏
    // 规则参考: 
    void init();

    // 开始游戏主循环
    void run();

    // 处理回合逻辑
    // 规则参考: 
    void playTurn(Controller& controller);

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

    // 供 Controller 使用：获取当前时代卡牌结构
    CardStructure& getCardStructure() { return *cardStructure; } 

    // 供 Card Effect 使用：
    void setIsGameOver(bool status) { isGameOver = status; }

    // 供 Controller 使用：三个核心动作
    void takeCard(int pos, Player& player);
    void buildWonder(int wonderIdx, int pos, Player& player);
    void discardForCoins(int pos, Player& player);
};