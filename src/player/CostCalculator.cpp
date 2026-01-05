#include "CostCalculator.h"
#include "player/Player.h"
#include "cards/Card.h"
#include <algorithm>
#include <map>

bool CostCalculator::is_tradable_resource(Resource res) {
    return (res == Resource::WOOD || res == Resource::CLAY || res == Resource::STONE ||
            res == Resource::GLASS || res == Resource::PAPYRUS);
}

int CostCalculator::calculate_trade_cost(const Player& buyer, const Player& seller, Resource res) {
    int base = buyer.get_trade_cost(res);
    if (base == 1) return 1;
    
    // 判定资源所属颜色分类 (WOOD, CLAY, STONE 为棕色，其余为灰色)
    Color target_col = (res == Resource::GLASS || res == Resource::PAPYRUS) ? Color::GREY : Color::BROWN;
    return base + seller.get_card_count_by_color(target_col);
}

CostCalculator::BuildCostResult CostCalculator::calculate_build_cost(
    const Player& player, const Player& opponent, const Card& card
) {
    BuildCostResult result;
    result.can_build = true;
    result.total_coin_cost = 0;

    // 1. 连锁免费判定
    if (card.can_be_free(player)) {
        result.is_free_by_chain = true;
        return result;
    }

    // 2. 初始金币成本
    auto it_coin = card.cost.find(Resource::COIN);
    if (it_coin != card.cost.end()) {
        result.total_coin_cost += it_coin->second;
    }

    // 3. 建立临时缺口统计 Map
    std::map<Resource, int> current_shortages;
    for (auto const& [res, req] : card.cost) {
        if (res == Resource::COIN) continue;
        int produced = player.get_resource(res);
        if (produced < req) {
            current_shortages[res] = req - produced;
        }
    }

    // 4. 多选一资源抵扣 (Safe Loop)
    const auto& wildcards = player.get_wildcard_resources();
    for (const auto& options : wildcards) {
        bool used_this_wildcard = false;
        for (Resource opt : options) {
            if (!used_this_wildcard && current_shortages.count(opt) && current_shortages[opt] > 0) {
                current_shortages[opt]--;
                used_this_wildcard = true; // 确保一张多选一卡只抵扣一个资源
            }
        }
    }

    // 5. 计算剩余缺口的购买费
    for (auto const& [res, amt] : current_shortages) {
        if (amt <= 0) continue;
        if (is_tradable_resource(res)) {
            int price = calculate_trade_cost(player, opponent, res);
            result.total_coin_cost += (price * amt);
        } else {
            result.can_build = false; 
            return result;
        }
    }

    // 6. 最终余额判定
    if (player.get_coins() < result.total_coin_cost) {
        result.can_build = false;
    }

    return result;
}

bool CostCalculator::can_afford_with_trade(const Player& player, const Player& opponent, const Card& card) {
    return calculate_build_cost(player, opponent, card).can_build;
}

bool CostCalculator::execute_build(Player& player, const Player& opponent, const Card& card) {
    BuildCostResult res = calculate_build_cost(player, opponent, card);
    if (!res.can_build) return false;
    
    // 执行扣款
    if (res.total_coin_cost > 0) {
        if (!player.spend_coins(res.total_coin_cost)) return false;
    }
    return true;
}