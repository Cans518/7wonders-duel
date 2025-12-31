#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

// 前向声明，减少头文件相互依赖
class Player;
class Game;

// 统一颜色枚举
enum class Color { BROWN, GREY, BLUE, YELLOW, RED, GREEN, PURPLE };

// 统一资源枚举（涵盖了你代码中出现的所有类型）
enum class Resource {
    WOOD, STONE, CLAY, ORE, BRICK,
    PAPYRUS, GLASS, CLOTH,
    COIN,
    SCIENCE_GEAR, SCIENCE_COMPASS, SCIENCE_TABLET
};

// 补齐进展标记定义，确保 Game 逻辑完整
enum class ProgressToken {
    P_AGRICULTURE, P_ARCHITECTURE, P_ECONOMY, P_LAW, P_MASONRY,
    P_MATHEMATICS, P_PHILOSOPHY, P_STRATEGY, P_THEOLOGY, P_URBANISM
};

class Card {
public:
    // 定义效果函数的类型：接受玩家引用和游戏逻辑引用
    using CardEffect = std::function<void(Player&, Game&)>;

    // 成员变量
    std::string name;
    int age;
    Color color;
    std::map<Resource, int> cost;
    std::vector<std::string> chain_prerequisites; // 免费建造的前置卡牌名
    std::string chain_provides;                  // 本卡提供的免费建造符号
    bool is_face_up;                             // 在金字塔布局中是否翻开
    CardEffect effect;                           // Lambda 存储

    // 构造函数
    Card(std::string n, int a, Color c, std::map<Resource, int> co,
         std::vector<std::string> pre, std::string pro, CardEffect eff);

    // 逻辑函数
    bool can_build_free(const Player& player) const;
    void apply_effect(Player& player, Game& game);
};

// 工厂函数：创建当前游戏所有的卡牌池
std::vector<std::unique_ptr<Card>> createAllCards();