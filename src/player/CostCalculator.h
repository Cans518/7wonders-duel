#ifndef COSTCALCULATOR_H
#define COSTCALCULATOR_H

#include "Player.h"
#include "../cards/Card.h"
#include <map>
#include <vector>

/**
 * CostCalculator 类：计算建造成本和交易成本
 * 核心功能：
 * 1. 计算交易成本：购买资源费用 = 2 + 对手拥有该类褐色/灰色卡数量（若有贸易折扣则为1）
 * 2. 计算总建造成本：考虑连锁建造（免费）、自有资源、需要购买的资源、金币成本
 * 3. 判断是否能建造卡牌
 */
class CostCalculator {
public:
    //计算从对手购买单个资源的成本
    static int calculateTradeCost(const Player& buyer, const Player& seller, Resource resource);

    struct BuildCostResult {
        bool canBuild;                          // 是否能建造
        bool isFreeByChain;                     // 是否通过连锁建造免费
        int totalCoinCost;                      // 总金币成本
        std::map<Resource, int> missingResources;  // 缺少的资源
        std::map<Resource, int> resourcesToBuy;    // 需要购买的资源
    };

    //计算建造成本
    static BuildCostResult calculateBuildCost(
        const Player& player, 
        const Player& opponent, 
        const Card& card
    );

    //判断是否能负担得起建造成本
    static bool canAffordWithTrade(
        const Player& player, 
        const Player& opponent, 
        const Card& card
    );

    //执行建造
    static bool executeBuild(
        Player& player, 
        const Player& opponent, 
        const Card& card
    );

private:
    //判断资源是否为褐色/灰色资源（可交易资源）
    static bool isTradableResource(Resource resource);
};

#endif // COSTCALCULATOR_H
