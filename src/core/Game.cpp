#include "Game.h"
// 引入 Mock 类或者队友写好的头文件
// 目前用 Mock 来测试
#include "../MockClasses.h" 

#include <iostream>

Game* Game::instance = nullptr;

Game& Game::getInstance() {
    if (instance == nullptr) {
        instance = new Game();
    }
    return *instance;
}

Game::Game() {
    board = std::make_unique<Board>();
    isGameOver = false;
    currentAge = 1;
    currentPlayerIdx = 0; // 默认P1先手，之后根据军事决定 
}

void Game::init() {
    std::cout << "[Game] Initializing 7 Wonders Duel..." << std::endl;
    
    // /// [DEPENDENCY] 可能需要更改: Player 构造函数
    // 创建两个玩家
    players.push_back(std::make_shared<Player>("Player 1"));
    players.push_back(std::make_shared<Player>("Player 2"));

    // 每个玩家拿7块钱
    for(auto& p : players) {
        p->addCoins(7); 
    }

    // TODO: Member 2 需要在这里初始化 Age 1 的卡牌金字塔 
    // CardStructure::loadAge(1); 
}

void Game::run() {
    init();

    while (!isGameOver && currentAge <= 3) {
        std::cout << "\n--- Start of Age " << currentAge << " ---" << std::endl;
        
        // 模拟一个时代的流程：假设每个时代有20张卡 
        // 这里的循环条件应该是: while(!cardStructure.isEmpty())
        // 为了演示，我们模拟几回合
        int turns = 0;
        while (turns < 6 && !isGameOver) { 
            playTurn();
            
            if (checkSupremacyVictory()) {
                isGameOver = true;
                break;
            }
            turns++;
        }

        if (!isGameOver) {
            endAge();
        }
    }

    // 如果没人突袭胜利，计算分数
    if (!isGameOver) {
        std::cout << "[Game] Civilian Victory Calculation..." << std::endl;
        // 计算分数的逻辑...
    }
    
    std::cout << "[Game] Game Over." << std::endl;
}

void Game::playTurn() {
    Player* curr = getCurrentPlayer();
    std::cout << "\n[Turn] It is " << curr->getName() << "'s turn." << std::endl;

    // 1. 获取输入 (Controller/View) - 这里简化为模拟
    // 假设玩家选择了一张卡并建造
    
    // /// [DEPENDENCY] 可能需要更改: 资源检查逻辑
    // Member 3 的 Player 类应该有 checkResources(cost) 方法
    
    // 2. 执行动作 (建造/弃牌/奇迹) 
    std::cout << "  Action: Constructing a Building..." << std::endl;
    
    // 3. 检查卡牌效果 (Member 2 的 Card 类)
    // 如果是军事卡 (Red)，移动棋子 
    // 模拟：假设这张卡有 1 个盾牌
    bool supremacy = board->movePawn(currentPlayerIdx == 0 ? -1 : 1); 
    
    if (supremacy) {
        isGameOver = true;
        std::cout << "  Military Supremacy Achieved!" << std::endl;
        return;
    }

    // 4. 切换玩家
    // 注意：有些奇迹让玩家 "Play Again" ，这里先略过
    currentPlayerIdx = (currentPlayerIdx + 1) % 2;
}

bool Game::checkSupremacyVictory() {
    // 军事胜利已经在 movePawn 里检查了返回 true/false
    // 这里主要检查科技胜利 
    
    // /// [DEPENDENCY] 可能需要更改: 获取玩家科技符号数量
    // if (getCurrentPlayer()->getScienceSymbols().size() >= 6) return true;
    
    return false;
}

void Game::endAge() {
    std::cout << "[Game] End of Age " << currentAge << std::endl;
    currentAge++;
    
    // 军事弱的玩家决定下一轮谁先手
    int pawnPos = board->getPawnPosition();
    if (pawnPos < 9) {
        // P2 弱 (棋子在 P2 这边)，P2 选
        std::cout << "  Pawn is on P1 side. P1 chooses who starts." << std::endl; 
        // 实际上逻辑是 Pawn < 9 (靠近0/P1) -> P1 被压制 -> P1 选 (原文规则可能有误或者我理解反了，按照PDF : "weaker military if conflict pawn is on their side")
        // 如果 0 是 P1 首都，9 是中间。Pawn=5 在 P1 侧。 P1 是 weak。
    } else if (pawnPos > 9) {
        std::cout << "  Pawn is on P2 side. P2 chooses who starts." << std::endl;
    } else {
        std::cout << "  Pawn in middle. Last player chooses." << std::endl;
    }
    
    // TODO: Member 2 加载下一个时代的卡牌结构 
}

Player* Game::getCurrentPlayer() {
    return players[currentPlayerIdx].get();
}

Player* Game::getOpponent() {
    return players[(currentPlayerIdx + 1) % 2].get();
}