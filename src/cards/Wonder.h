#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
// 确保 Resource 枚举在 Player.h 或某处已定义
#include "../player/Player.h"

class Game; // 前向声明

class Wonder {
public:
    // 定义别名使代码更整洁
    using WonderEffect = std::function<void(Player& self, Player& opponent, Game& game)>;

    std::string name;
    std::map<Resource, int> cost;
    WonderEffect effect;

    // 修改 auto 为明确的 std::function 类型
    Wonder(std::string n, std::map<Resource, int> co, WonderEffect eff);
};

// 声明工厂函数
std::vector<Wonder> createAllWonders();