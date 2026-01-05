#ifndef CARD_H
#define CARD_H

#include "Types.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

class Player;
class Game;

class Card {
public:
    using CardEffect = std::function<void(Player&, Game&)>;

    // --- 结构体：特殊奖励 (用于黄色/紫色卡) ---
    struct SpecialReward {
        bool active;
        Color target_color;   // 关联哪种颜色的牌
        int coins_per_card;   // 建造时即时给的钱
        int vp_per_card;      // 游戏结束给的分
        bool count_wonders;   // 是否关联奇迹数量
        bool count_both;      // 是否计算双方玩家的牌 (针对公会)

        SpecialReward() : active(false), target_color(Color::BROWN), coins_per_card(0), vp_per_card(0), count_wonders(false), count_both(false) {}
        SpecialReward(bool a, Color c, int coins, int vp, bool wonders, bool both = false)
            : active(a), target_color(c), coins_per_card(coins), vp_per_card(vp), count_wonders(wonders), count_both(both) {}
    };

    // --- 基础属性 ---
    std::string name;
    int age;
    Color color;
    std::map<Resource, int> cost;
    
    // --- 结构化效果 ---
    int victory_points = 0;
    int shields = 0;
    Resource science_symbol = Resource::COIN; // 若非科技牌则为 COIN

    // --- 连锁系统 ---
    LinkSymbol link_prerequisite = LinkSymbol::NONE; 
    LinkSymbol link_provides = LinkSymbol::NONE;     

    SpecialReward special_reward;
    CardEffect immediate_func; // 用于处理 Reserve 或产出资源等逻辑
    bool is_face_up = false;

    Card(std::string n, int a, Color c);

    bool can_be_free(const Player& p) const;
    void apply_effect(Player& p, Game& g) const;
};

std::vector<std::unique_ptr<Card>> createAllCards();

#endif