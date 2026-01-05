#ifndef WONDER_H
#define WONDER_H

#include <string>
#include <map>
#include <vector>
#include <functional>
#include "Types.h"

// 前向声明
class Player;
class Game;

class Wonder {
public:
    // 定义奇迹特殊效果的 Lambda 类型：(自己, 对手, 游戏实例)
    using WonderEffect = std::function<void(Player& self, Player& opponent, Game& game)>;

    std::string name;
    std::map<Resource, int> cost;
    
    // 结构化数据 (参考规则书 P17)
    int victory_points = 0;
    int shields = 0;
    bool is_built = false;

    // 存储特殊逻辑 (如“再来一回合”、“拆牌”、“选择进展标记”)
    WonderEffect effect;

    Wonder(std::string n, std::map<Resource, int> co, WonderEffect eff = nullptr);
};

// 工厂函数：创建对决版全部 12 张奇迹卡
std::vector<Wonder> createAllWonders();

#endif