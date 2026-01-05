#include "Game.h"
#include "Board.h"
#include "player/Player.h"
#include "player/CostCalculator.h"
#include "cards/Card.h"
#include "cards/CardStructure.h"
#include "view/Ctrller.h"
#include <iostream>
#include <algorithm>
#include <random>

// 1. 核心修复：初始化单例静态指针 (解决 Error 1 / ld 报错)
Game* Game::instance = nullptr;

Game& Game::getInstance() {
    if (instance == nullptr) {
        instance = new Game();
    }
    return *instance;
}

Game::Game() : board(std::make_unique<Board>()), 
               current_age(1), 
               current_player_idx(0), 
               is_game_over(false), 
               extra_turn_triggered(false) {}

void Game::init() {
    std::cout << "[Game] Initializing 7 Wonders Duel..." << std::endl;
    
    // 初始化玩家
    players.clear();
    players.push_back(std::make_shared<Player>("Player 1"));
    players.push_back(std::make_shared<Player>("Player 2"));

    // 规则书 P6：初始金币为 7
    for(auto& p : players) {
        p->add_coins(7); 
    }

    // 分发奇迹 (每人 4 个)
    distribute_wonders();

    // 构建初始时代布局
    setup_age_structure(1);
}

void Game::distribute_wonders() {
    auto all_wonders = createAllWonders();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_wonders.begin(), all_wonders.end(), g);
    
    // 规则 P7：给 P1 前 4 个，P2 后 4 个
    for(int i = 0; i < 4; ++i) players[0]->add_wonder(all_wonders[i]);
    for(int i = 4; i < 8; ++i) players[1]->add_wonder(all_wonders[i]);
}

void Game::setup_age_structure(int age) {
    auto all_cards = createAllCards();
    std::vector<std::unique_ptr<Card>> age_deck;
    for (auto& c : all_cards) {
        if (c->age == age) age_deck.push_back(std::move(c));
    }

    // 增加一个调试打印，看看实际找到了多少张牌
    std::cout << "[DEBUG] Loading Age " << age << ", found " << age_deck.size() << " cards." << std::endl;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(age_deck.begin(), age_deck.end(), g);

    // 必须确保正好 20 张
    if (age_deck.size() > 20) {
        age_deck.resize(20); // 使用 resize 更安全
    } else if (age_deck.size() < 20) {
        std::cerr << "Fatal Error: Not enough cards for Age " << age << std::endl;
        exit(1);
    }
    
    cardStructure = std::make_unique<CardStructure>(age, std::move(age_deck));
}

// --- 核心动作 1：购买/建造卡牌 ---
bool Game::take_card(int pos, Player& player) {
    const Card* card_ptr = cardStructure->get_card(pos);
    if (!card_ptr) return false;

    // 支付逻辑（调用 CostCalculator，含连锁检查）
    if (!CostCalculator::execute_build(player, *get_opponent(), *card_ptr)) {
        return false;
    }

    // 从金字塔移走卡牌并获取所有权
    std::unique_ptr<Card> card = cardStructure->take_card(pos);

    // 执行结构化效果 (VP, 盾牌, 符号)
    card->apply_effect(player, *this);
    player.add_built_card(card->name, card->color);

    if (check_supremacy_victory()) return true;

    handle_turn_switch();
    return true;
} 

// --- 核心动作 2：弃牌换钱 ---
void Game::discard_for_coins(int pos, Player& player) {
    std::unique_ptr<Card> card = cardStructure->take_card(pos);
    if (!card) return;

    // 规则 P10：基础 2 金 + 拥有的黄卡数量
    int gain = 2 + player.count_yellow();
    player.add_coins(gain);
    
    discard_pile.push_back(std::move(card));
    std::cout << "[Game] " << player.get_name() << " gained " << gain << " coins." << std::endl;
    
    handle_turn_switch();
}

// --- 核心动作 3：建造奇迹 ---
bool Game::build_wonder(int wonder_idx, int pos, Player& player) {
    Wonder& wonder = player.get_wonder(wonder_idx);
    
    // 检查奇迹状态及金字塔是否有地基
    if (wonder.is_built || !cardStructure->get_card(pos)) return false;

    // 取走卡牌作为地基（面朝下）
    std::unique_ptr<Card> foundation = cardStructure->take_card(pos);
    discard_pile.push_back(std::move(foundation));

    // 应用奇迹结构化效果
    player.add_victory_points(wonder.victory_points);
    if (wonder.shields > 0) move_pawn(wonder.shields);
    
    // 执行 Lambda 效果 (如 Appian Way 扣钱)
    if (wonder.effect) {
        wonder.effect(player, *get_opponent(), *this);
    }

    wonder.is_built = true;
    player.increment_wonder_count();

    handle_turn_switch();
    return true;
}

void Game::handle_turn_switch() {
    if (extra_turn_triggered) {
        std::cout << ">>> EXTRA TURN! <<<" << std::endl;
        extra_turn_triggered = false; 
    } else {
        current_player_idx = (current_player_idx + 1) % 2;
    }
}

void Game::move_pawn(int steps) {
    // P1 推向 P2 (+), P2 推向 P1 (-)
    int direction = (current_player_idx == 0) ? 1 : -1;
    if (board->move_pawn(steps * direction, *players[0], *players[1])) {
        is_game_over = true;
    }
}

bool Game::check_supremacy_victory() {
    // 军事压制
    if (board->get_pawn_position() <= 0 || board->get_pawn_position() >= 18) return true;
    // 科技压制
    if (get_current_player()->get_unique_science_count() >= 6) return true;
    return false;
}

void Game::run() {
    init();
    Controller controller(*this); 

    while (!is_game_over && current_age <= 3) {
        while (!cardStructure->is_empty() && !is_game_over) { 
            controller.player_turn(*get_current_player());
            if (check_supremacy_victory()) {
                is_game_over = true;
                break;
            }
        }

        if (!is_game_over) {
            current_age++;
            if (current_age <= 3) {
                std::cout << "\n--- Starting Age " << current_age << " ---" << std::endl;
                setup_age_structure(current_age);
            }
        }
    }
    
    Player* winner = players[0]->calculate_final_score() > players[1]->calculate_final_score() ? players[0].get() : players[1].get();
    std::cout << "\nGAME OVER! Winner: " << winner->get_name() << std::endl;
}

// --- Getter 组 (对齐 snake_case) ---

Player* Game::get_current_player() { return players[current_player_idx].get(); }
Player* Game::get_opponent() { return players[(current_player_idx + 1) % 2].get(); }

Player* Game::get_opponent(Player& p) {
    return (players[0].get() == &p) ? players[1].get() : players[0].get();
}

// --- 奇迹效果回调接口 ---

void Game::trigger_progress_token_selection(Player& p, int count) {
    std::cout << "[INFO] Progress Token Selection triggered for " << p.get_name() << std::endl;
}

void Game::trigger_build_from_discard(Player& p) {
    std::cout << "[INFO] Build from Discard triggered for " << p.get_name() << std::endl;
}

std::vector<Card*> Game::get_discard_pile_view() {
    std::vector<Card*> view;
    for(auto& c : discard_pile) view.push_back(c.get());
    return view;
}

void Game::check_science_victory(Player& p) {
    if (p.get_unique_science_count() >= 6) is_game_over = true;
}