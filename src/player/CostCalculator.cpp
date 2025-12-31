#include "CostCalculator.h"
#include <algorithm>

// 辅助函数：判断资源是否为可交易资源（褐色或灰色）
bool CostCalculator::isTradableResource(Resource resource) {
    return resource == Resource::WOOD || 
           resource == Resource::CLAY || 
           resource == Resource::ORE || 
           resource == Resource::STONE ||
           resource == Resource::GLASS || 
           resource == Resource::CLOTH || 
           resource == Resource::PAPYRUS;
}

// 交易成本计算
int CostCalculator::calculateTradeCost(const Player& buyer, const Player& seller, Resource resource) {
    if (buyer.has_trading_post(resource)) {
        return 1;
    }
    int baseCost = 2;
    int sellerCardCount = seller.get_resource_producing_card_count(resource);
    return baseCost + sellerCardCount;
}

// 建造成本计算
CostCalculator::BuildCostResult CostCalculator::calculateBuildCost(
    const Player& player, 
    const Player& opponent, 
    const Card& card
) {
    BuildCostResult result;
    result.canBuild = true;
    result.isFreeByChain = false;
    result.totalCoinCost = 0;

    // 1. 检查连锁建造
    for (const auto& pre : card.chain_prerequisites) {
        if (player.has_card(pre)) {
            result.isFreeByChain = true;
            return result;
        }
    }

    // 2. 处理金币成本
    auto coinIt = card.cost.find(Resource::COIN);
    if (coinIt != card.cost.end()) {
        result.totalCoinCost += coinIt->second;
    }

    // 3. 计算资源缺口
    std::map<Resource, int> shortages;
    for (const auto& [resource, requiredAmount] : card.cost) {
        if (resource == Resource::COIN) continue;
        int producedAmount = player.get_resource(resource);
        if (producedAmount < requiredAmount) {
            shortages[resource] = requiredAmount - producedAmount;
        }
    }

    // 4. 使用二选一资源抵扣
    auto wildcards = player.get_wildcard_resources();
    for (const auto& options : wildcards) {
        for (Resource res : options) {
            if (shortages[res] > 0) {
                shortages[res]--;
                if (shortages[res] == 0) shortages.erase(res);
                break;
            }
        }
    }

    // 5. 计算购买费用
    for (const auto& [resource, amount] : shortages) {
        result.missingResources[resource] = amount;
        if (isTradableResource(resource)) {
            int tradeCostPerUnit = calculateTradeCost(player, opponent, resource);
            result.resourcesToBuy[resource] = amount;
            result.totalCoinCost += tradeCostPerUnit * amount;
        } else {
            result.canBuild = false;
            return result;
        }
    }

    // 6. 检查金币
    if (player.get_coins() < result.totalCoinCost) {
        result.canBuild = false;
    }
    return result;
}

bool CostCalculator::canAffordWithTrade(
    const Player& player, 
    const Player& opponent, 
    const Card& card
) {
    return calculateBuildCost(player, opponent, card).canBuild;
}

bool CostCalculator::executeBuild(
    Player& player, 
    const Player& opponent, 
    const Card& card
) {
    BuildCostResult result = calculateBuildCost(player, opponent, card);
    if (!result.canBuild) return false;
    if (result.totalCoinCost > 0) {
        if (!player.spend_coins(result.totalCoinCost)) return false;
    }
    return true;
}
