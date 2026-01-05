#include "Player.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

// --- 构造函数 ---
// 初始化所有基础数值，确保不产生随机垃圾值
Player::Player(const std::string& playerName, PlayerType playerType) 
    : name(playerName), type(playerType), coins(7), 
      military_tokens(0), victory_points(0), built_wonders_count(0) {
}

// --- 经济管理 ---

void Player::add_coins(int amount) {
    // 允许传入负数进行扣款，并确保余额不会低于 0（规则书 P14 保护逻辑）
    coins += amount;
    if (coins < 0) coins = 0; 
}

bool Player::spend_coins(int amount) {
    if (coins < amount) return false;
    coins -= amount;
    return true;
}

// --- 资源产出与交易逻辑 ---

void Player::add_resource(Resource res, int amount) {
    if (amount > 0) resources[res] += amount;
}

int Player::get_resource(Resource res) const {
    // 严禁使用 resources[res]，因为它会在 key 不存在时插入新条目
    auto it = resources.find(res);
    if (it != resources.end()) {
        return it->second;
    }
    return 0;
}

void Player::add_resource_choice(const std::set<Resource>& options) {
    if (!options.empty()) {
        wildcard_resources.push_back(options);
    }
}

void Player::set_fixed_trade_cost(Resource res, int cost) {
    fixed_trade_costs[res] = cost;
}

int Player::get_trade_cost(Resource res) const {
    // 如果玩家拥有对应的黄色“储备卡”，该资源的交易基础费固定为 1（规则书 P8）
    auto it = fixed_trade_costs.find(res);
    if (it != fixed_trade_costs.end()) {
        return it->second;
    }
    return 2; // 默认规则：基础费为 2
}

// --- 卡牌管理与统计 ---

void Player::add_built_card(const std::string& cardName, Color cardColor) {
    built_card_names.push_back(cardName);
    cards_by_color[cardColor]++;
}

bool Player::has_card(const std::string& cardName) const {
    return std::find(built_card_names.begin(), built_card_names.end(), cardName) != built_card_names.end();
}

int Player::get_card_count_by_color(Color color) const {
    auto it = cards_by_color.find(color);
    if (it != cards_by_color.end()) {
        return it->second;
    }
    return 0;
}

// 统计快捷函数实现 (用于时代 III / 公会卡关联计分)
int Player::count_brown() const  { return get_card_count_by_color(Color::BROWN); }
int Player::count_grey() const   { return get_card_count_by_color(Color::GREY); }
int Player::count_yellow() const { return get_card_count_by_color(Color::YELLOW); }
int Player::count_blue() const   { return get_card_count_by_color(Color::BLUE); }
int Player::count_green() const  { return get_card_count_by_color(Color::GREEN); }
int Player::count_red() const    { return get_card_count_by_color(Color::RED); }
int Player::count_purple() const { return get_card_count_by_color(Color::PURPLE); }

// --- 连锁符号逻辑 (Linking) ---

void Player::add_chain_symbol(LinkSymbol symbol) {
    if (symbol != LinkSymbol::NONE) {
        owned_link_symbols.insert(symbol);
    }
}

bool Player::has_chain_symbol(LinkSymbol symbol) const {
    return owned_link_symbols.count(symbol) > 0;
}

// --- 奇迹管理 (彻底修复 undefined reference 报错) ---

void Player::add_wonder(const Wonder& w) {
    wonders.push_back(w);
}

// 补全 get_wonder 实现
Wonder& Player::get_wonder(int idx) {
    if (idx < 0 || idx >= (int)wonders.size()) {
        throw std::out_of_range("Player::get_wonder - Index out of range");
    }
    return wonders[idx];
}

int Player::count_wonder_stages() const {
    return built_wonders_count;
}

// 补全 increment_wonder_count 实现
void Player::increment_wonder_count() {
    built_wonders_count++;
}

// --- 科技、军事与特殊效果 ---

void Player::add_science_symbol(Resource symbol) {
    // 判定是否属于科技符号区间 (COMPASS 到 LAW)
    if (symbol >= Resource::COMPASS && symbol <= Resource::LAW) {
        science_symbols.insert(symbol);
    }
}

int Player::get_unique_science_count() const {
    // 返回 set 的大小即为不同科技符号的数量 (规则书 P12)
    return static_cast<int>(science_symbols.size());
}

void Player::destroy_card_by_color(Color color) {
    // 奇迹破坏效果：减少对手某色卡牌计数
    auto it = cards_by_color.find(color);
    if (it != cards_by_color.end() && it->second > 0) {
        it->second--;
        std::cout << "[Effect] " << name << " lost a card of color " << (int)color << std::endl;
    }
}

// --- 最终结算 ---

int Player::calculate_final_score() const {
    // 1. 基础胜利点数 (来自蓝卡、绿卡、黄卡、红卡、奇迹的直接加分)
    int total = victory_points;
    // 2. 现金换分：每 3 元换 1 分 (规则书 P13)
    total += (coins / 3);
    return total;
}