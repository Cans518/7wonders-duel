#include "Player.h"
#include <algorithm>

// 构造函数

Player::Player(const std::string& playerName, PlayerType playerType) 
    : name(playerName), type(playerType), coins(7), militaryTokens(0), victoryPoints(0) {
}

// 资源管理 

void Player::add_resource(Resource resource, int amount) {
    if (amount > 0) {
        resources[resource] += amount;
    }
}

int Player::get_resource(Resource resource) const {
    auto it = resources.find(resource);
    return it != resources.end() ? it->second : 0;
}

bool Player::has_trading_post(Resource resource) const {
    return tradingPosts.count(resource) > 0;
}

void Player::add_trading_post(Resource resource) {
    tradingPosts.insert(resource);
}

void Player::add_wildcard_resource(const std::set<Resource>& options) {
    wildcardResources.push_back(options);
}

bool Player::has_enough_resource(Resource resource, int amount) const {
    return get_resource(resource) >= amount;
}

// 金币管理 

int Player::get_coins() const {
    return coins;
}

bool Player::spend_coins(int amount) {
    if (coins < amount) {
        return false;
    }
    coins -= amount;
    return true;
}

void Player::add_coins(int amount) {
    if (amount > 0) {
        coins += amount;
    }
}

// 卡牌管理 

void Player::add_built_card(const std::string& cardName, Color cardColor) {
    builtCards.push_back(cardName);
    cardsByColor[cardColor]++;
}

bool Player::has_card(const std::string& cardName) const {
    return std::find(builtCards.begin(), builtCards.end(), cardName) != builtCards.end();
}

const std::vector<std::string>& Player::get_built_cards() const {
    return builtCards;
}

int Player::get_card_count_by_color(Color color) const {
    auto it = cardsByColor.find(color);
    return it != cardsByColor.end() ? it->second : 0;
}

// 资源产出卡统计

void Player::add_resource_producing_card(Resource resource) {
    resourceProducingCards[resource]++;
}

int Player::get_resource_producing_card_count(Resource resource) const {
    auto it = resourceProducingCards.find(resource);
    return it != resourceProducingCards.end() ? it->second : 0;
}

// 军事 

void Player::add_shield(int amount) {
    militaryTokens += amount;
}

int Player::get_military_tokens() const {
    return militaryTokens;
}

bool Player::has_military_victory() const {
    return militaryTokens >= 9 || militaryTokens <= -9;
}

//科技

void Player::add_science_symbol(Resource symbol) {
    if (symbol == Resource::SCIENCE_COMPASS || 
        symbol == Resource::SCIENCE_GEAR || 
        symbol == Resource::SCIENCE_TABLET) {
        scienceSymbols.insert(symbol);
    }
}

const std::set<Resource>& Player::get_science_symbols() const {
    return scienceSymbols;
}

bool Player::has_science_victory() const {
    if (scienceSymbols.size() >= 6) {
        return true;
    }
    return false;
}

// 胜利点数 

void Player::add_victory_points(int amount) {
    victoryPoints += amount;
}

int Player::get_victory_points() const {
    return victoryPoints;
}

int Player::calculate_final_score() const {
    return victoryPoints + (coins / 3);
}