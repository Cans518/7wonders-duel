#ifndef PLAYER_H
#define PLAYER_H

#include "Types.h" 
#include "cards/Wonder.h"
#include <map>
#include <vector>
#include <string>
#include <set>
#include <memory>

class Player {
private:
    std::string name;
    PlayerType type;
    int coins;
    int military_tokens;
    int victory_points;
    int built_wonders_count; 

    std::map<Resource, int> resources;                  
    std::vector<std::set<Resource>> wildcard_resources;  
    std::map<Resource, int> fixed_trade_costs;          

    std::map<Color, int> cards_by_color;                
    std::vector<std::string> built_card_names;          
    std::set<LinkSymbol> owned_link_symbols;            
    std::set<Resource> science_symbols;                 
    std::vector<Wonder> wonders;                        

public:
    Player(const std::string& playerName = "Player", PlayerType playerType = PlayerType::HUMAN);

    // --- 基础信息 ---
    std::string get_name() const { return name; } // 短函数可以留在.h
    int get_coins() const { return coins; }
    void add_coins(int amount); 
    bool spend_coins(int amount);
    void add_victory_points(int amount) { victory_points += amount; }
    int get_victory_points() const { return victory_points; }

    // --- 资源与交易 ---
    void add_resource(Resource res, int amount);
    int get_resource(Resource res) const;
    void add_resource_choice(const std::set<Resource>& options);
    const std::vector<std::set<Resource>>& get_wildcard_resources() const { return wildcard_resources; }
    
    // 重点：这里只留声明，不要写大括号实现
    void set_fixed_trade_cost(Resource res, int cost);
    int get_trade_cost(Resource res) const;

    // --- 卡牌管理 ---
    void add_built_card(const std::string& cardName, Color cardColor);
    int get_card_count_by_color(Color color) const;
    bool has_card(const std::string& cardName) const;
    
    // 重点：只留声明
    void add_chain_symbol(LinkSymbol symbol);
    bool has_chain_symbol(LinkSymbol symbol) const;

    // --- 统计快捷函数 ---
    int count_brown() const;
    int count_grey() const;
    int count_yellow() const;
    int count_blue() const;
    int count_green() const;
    int count_red() const;
    int count_purple() const;

    // --- 奇迹管理 ---
    void add_wonder(const Wonder& w);
    Wonder& get_wonder(int idx);
    int count_wonder_stages() const;
    void increment_wonder_count();

    // --- 军事与科技 ---
    void add_shield(int amount);
    int get_military_tokens() const;
    void add_science_symbol(Resource symbol);
    int get_unique_science_count() const;
    void destroy_card_by_color(Color color);           

    int calculate_final_score() const;
};

#endif