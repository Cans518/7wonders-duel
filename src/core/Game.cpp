#include "Game.h"
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

    // TODO: 替代之前 Mock 的 CardStructure 初始化
    // 假设 Member 2 提供了一个 CardStructureFactory
    // cardStructure = CardStructureFactory::createAge(1);
    
    // 暂时用一个空的 CardStructure 来通过编译
    std::vector<std::unique_ptr<Card>> empty_deck;
    cardStructure = std::make_unique<CardStructure>(1, std::move(empty_deck));
}

//新增：takeCard 核心逻辑
void Game::takeCard(int pos, Player& player) {
    std::cout << "[Game] " << player.getName() << " attempting to take card at position " << pos << std::endl;
    
    // 1. 取出卡牌 (调用 Member 2 的逻辑)
    std::unique_ptr<Card> card = cardStructure->take_card(pos); 
    
    // 2. 检查建造条件并支付
    if (!card->can_build_free(player)) {
        // 调用 Member 3 的支付逻辑
        // TODO: 检查资源和金币是否足够，如果不够则抛出错误
        //player.payCost(card->cost, *getOpponent());
    }
    
    // 3. 应用卡牌效果
    if (card->effect) {
        Player* opponent = getOpponent();
        card->effect(player, *opponent, *this);
    }
    
    // 4. 记录已建卡牌 (用于连锁和得分)
    // 调用 Member 3 的方法
    // player.addBuiltCard(card->name, card->color);
    
    // 5. 切换玩家
    currentPlayerIdx = (currentPlayerIdx + 1) % 2;
}

// TODO: 实现 buildWonder 和 discardForCoins
// buildWonder 和 discardForCoins 的实现逻辑类似 takeCard，但效果不同。
// 它们也必须在函数末尾调用 currentPlayerIdx = (currentPlayerIdx + 1) % 2; 切换玩家。


void Game::run() {
    init();

    // 🔴 步骤 1：在 Game::run() 中初始化 Controller
    Controller controller(*this); 

    while (!isGameOver && currentAge <= 3) {
        std::cout << "\n--- Start of Age " << currentAge << " ---" << std::endl;
        
        // 🔴 步骤 2：核心修正！循环条件不再是 turns < 6，而是卡牌结构为空
        while (!isGameOver && !cardStructure->is_empty()) { 
            
            // 🔴 步骤 3：直接调用 playTurn()，让它处理一个回合的逻辑
            playTurn(controller); // 传入 controller
            
            // 检查胜利条件依然保留
            if (checkSupremacyVictory()) {
                isGameOver = true;
                break;
            }
        }

    if (!isGameOver) {
            endAge();
        }
    
    std::cout << "[Game] Game Over." << std::endl;
    // ... (省略游戏结束后的算分和打印)
}
}

void Game::playTurn(Controller& controller) { 
    Player* curr = getCurrentPlayer(); // 获取当前玩家指针

    // 1. 打印提示
    std::cout << "\n[Turn] It is " << curr->getName() << "'s turn." << std::endl;
    
    // 🔴 核心功能：将控制权委托给 Controller
    // Controller::player_turn 负责：
    // - 展示游戏状态 (View)
    // - 读取用户输入 ("take 3", "discard 5" 等)
    // - 调用 Game::takeCard 或 Game::discardForCoins 来执行操作
    controller.player_turn(*curr); 
    
    // ⚠️ 注意：
    // 1. 之前所有的模拟代码（如 Action: Constructing a Building... 和 board->movePawn）全部被移除！
    // 2. 切换玩家的逻辑 (currentPlayerIdx = (currentPlayerIdx + 1) % 2) 必须被移除，
    //    它已经被移动到 Game::takeCard, Game::buildWonder, Game::discardForCoins 这三个方法中。
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