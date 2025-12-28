#include "CostCalculator.h"
#include <algorithm>

// 辅助函数：判断资源是否为可交易资源（褐色或灰色）
bool CostCalculator::isTradableResource(Resource resource) {
    return resource == Resource::WOOD || 
           resource == Resource::BRICK || 
           resource == Resource::ORE || 
           resource == Resource::STONE ||
           resource == Resource::GLASS || 
           resource == Resource::CLOTH || 
           resource == Resource::PAPYRUS;
}

// 交易成本计算
int CostCalculator::calculateTradeCost(const Player& buyer, const Player& seller, Resource resource) {
    if (buyer.hasTradingPost(resource)) {
        return 1;
    }
    int baseCost = 2;
    int sellerCardCount = seller.getResourceProducingCardCount(resource);
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
        if (player.hasBuilding(pre)) {
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
        int producedAmount = player.getResource(resource);
        if (producedAmount < requiredAmount) {
            shortages[resource] = requiredAmount - producedAmount;
        }
    }

    // 4. 使用二选一资源抵扣
    auto wildcards = player.getWildcardResources();
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
    if (player.getCoins() < result.totalCoinCost) {
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
        if (!player.spendCoins(result.totalCoinCost)) return false;
    }
    return true;
}
