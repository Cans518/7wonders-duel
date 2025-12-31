#include "Game.h"
#include "Board.h"
#include "../player/Player.h"
#include "../player/CostCalculator.h"
#include "../cards/Card.h"
#include "../cards/CardStructure.h"
#include "../view/Ctrller.h"
#include <iostream>
#include <algorithm>

// 单例实例初始化
Game* Game::instance = nullptr;

Game& Game::getInstance() {
    if (instance == nullptr) {
        instance = new Game();
    }
    return *instance;
}

Game::Game() : board(std::make_unique<Board>()), 
               currentAge(1), 
               currentPlayerIdx(0), 
               isGameOver(false), 
               extraTurnTriggered(false) {}

void Game::init() {
    std::cout << "[Game] Initializing 7 Wonders Duel..." << std::endl;
    
    // 初始化玩家
    players.clear();
    players.push_back(std::make_shared<Player>("Player 1"));
    players.push_back(std::make_shared<Player>("Player 2"));

    // 初始金币：根据规则通常是7元
    for(auto& p : players) {
        p->add_coins(7); 
    }

    // 初始化第一时代卡牌布局 (Member 2 需要提供 createAllCards 逻辑)
    // 这里假设逻辑已经迁移到符合 20 张牌的金字塔结构
    auto age1_deck = createAllCards(); // 实际应按时代筛选
    // 暂时模拟洗牌取20张
    cardStructure = std::make_unique<CardStructure>(1, std::move(age1_deck));
}

// 核心动作 1：购买/建造卡牌
bool Game::takeCard(int pos, Player& player) {
        // 1. 尝试从结构中移除并获取卡牌
        std::unique_ptr<Card> card = cardStructure->takeCard(pos);
        if (!card) return false;

        // 2. 利用 Member 3 提供的 executeBuild 直接处理支付逻辑
        // 它内部已经调用了 calculateBuildCost 和 player.spend_coins
        if (!CostCalculator::executeBuild(player, *getOpponent(), *card)) {
            // 如果买不起，把牌还回去（或者告知非法操作）
            // 注意：如果你从 cardStructure 拿走时已经删除了，这里需要更复杂的处理
            return false;
        }

        // 3. 执行 Member 2 的 Lambda 效果
        card->apply_effect(player, *this);

        // 4. 登记卡牌
        player.add_built_card(card->name, card->color);


        // 胜利检查：双重保险
        // 检查点 1：检查 Lambda 是否已经触发了胜利 (如科技 6 种)
        if (isGameOver) return true; 

        // 检查点 2：手动触发一次全局压制检查 (防止 Member 2 漏写)
        if (checkSupremacyVictory()) {
            isGameOver = true;
            return true;
        }

        // 6. 回合切换
        if (!extraTurnTriggered) {
            currentPlayerIdx = (currentPlayerIdx + 1) % 2;
        } else {
            std::cout << "[Game] Extra turn for " << player.get_name() << "!" << std::endl;
            extraTurnTriggered = false; 
        }
        return true;
    } 

// 核心动作 2：弃牌换钱
void Game::discardForCoins(int pos, Player& player) {
    try {
        std::unique_ptr<Card> card = cardStructure->takeCard(pos);
        if (!card) return;

        int coins = 2 + player.count_yellow_cards(); 
        player.add_coins(coins);
        
        // --- 关键修改：将弃掉的牌存入弃牌堆 ---
        discardPile.push_back(std::move(card));
        
        std::cout << player.get_name() << " discarded card for " << coins << " coins." << std::endl;
        
        currentPlayerIdx = (currentPlayerIdx + 1) % 2;
        extraTurnTriggered = false; 
    } catch (...) {}
}
// 核心动作 3：建造奇迹
void Game::buildWonder(int wonderIdx, int pos, Player& player) {
    // TODO: 实现奇迹建造逻辑
    // 1. 检查玩家是否还有奇迹位
    // 2. 计算奇迹成本并支付
    // 3. 从 cardStructure 中移除 pos 位置卡牌（作为奇迹地基）
    // 4. 触发奇迹效果
    // 5. 换人逻辑 (注意：有些奇迹会设置 extraTurnTriggered = true)
}

void Game::run() {
    init();
    Controller controller(*this); 

    while (!isGameOver && currentAge <= 3) {
        while (!cardStructure->isEmpty() && !isGameOver) { 
            playTurn(controller);
            if (checkSupremacyVictory()) {
                isGameOver = true;
                break;
            }
        }

        if (!isGameOver) {
            endAge();
        }
    }
    // TODO: 游戏结束最后的得分计算与结果展示
}

void Game::playTurn(Controller& controller) { 
    Player* curr = getCurrentPlayer();
    std::cout << "\n--- " << curr->get_name() << "'s Turn ---" << std::endl;
    
    // 委托给 Controller 处理 UI 和输入
    controller.player_turn(*curr); 
}

bool Game::checkSupremacyVictory() {
    // 1. 军事压制：棋子到达 0 或 18
    if (board->getPawnPosition() <= 0 || board->getPawnPosition() >= 18) {
        std::cout << "Military Supremacy Victory!" << std::endl;
        return true;
    }
    
    // 2. 科技压制：收集 6 种不同的科技符号
    if (getCurrentPlayer()->get_unique_science_count() >= 6) {
        std::cout << "Scientific Supremacy Victory!" << std::endl;
        return true;
    }
    
    return false;
}

void Game::endAge() {
    std::cout << "\n--- Age " << currentAge << " has ended ---" << std::endl;
    currentAge++;
    
    if (currentAge <= 3) {
        // TODO: 逻辑：军事落后方选择谁开始下一时代
        // 此处暂时简化为默认切换
        // cardStructure = CardStructureFactory::createAge(currentAge);
    }
}

Player* Game::getCurrentPlayer() {
    return players[currentPlayerIdx].get();
}

Player* Game::getOpponent() {
    return players[(currentPlayerIdx + 1) % 2].get();
}

void Game::movePawn(int steps) {
    if (!board) return;

    // 逻辑：
    // 如果当前玩家是 P1 (Idx 0)，军事推进意味着向 P2 的首都移动 (amount > 0)
    // 如果当前玩家是 P2 (Idx 1)，军事推进意味着向 P1 的首都移动 (amount < 0)
    int actualAmount = (currentPlayerIdx == 0) ? steps : -steps;

    // 获取玩家引用
    Player& p1 = *players[0];
    Player& p2 = *players[1];

    // 调用 Board 执行移动
    bool isVictory = board->movePawn(actualAmount, p1, p2);

    if (isVictory) {
        isGameOver = true;
        std::cout << "--- MILITARY SUPREMACY! Game Over ---" << std::endl;
    }
}

// 对应 Player::get_unique_science_count
bool Game::checkSupremacyVictory() {
    // 1. 军事
    if (board->getPawnPosition() <= 0 || board->getPawnPosition() >= 18) return true;
    
    // 2. 科技：对齐 get_unique_science_count
    if (getCurrentPlayer()->get_unique_science_count() >= 6) return true;
    
    return false;
}

// 1. 科技胜利检查：对应 Member 2 的 g.check_science_victory(s)
void Game::check_science_victory(Player& p) {
    // 对应你的 Player.h 中的 get_unique_science_count
    if (p.get_unique_science_count() >= 6) {
        isGameOver = true;
        std::cout << "--- 科技压制胜利！ " << p.get_name() << " 赢得了比赛 ---" << std::endl;
    }
}

// 进展标记随机抽取
std::vector<ProgressToken> Game::drawRandomProgressTokens(int count) {
    std::vector<ProgressToken> drawn;
    // 这里简单实现：如果池子里有就给
    for(int i=0; i<count && !progressTokenPool.empty(); ++i) {
        drawn.push_back(progressTokenPool.back());
        progressTokenPool.pop_back();
    }
    return drawn;
}