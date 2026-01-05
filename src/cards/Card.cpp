#include "Card.h"
#include "../player/Player.h"  // 必须包含以调用 Player 的方法
#include "../core/Game.h"      // 必须包含以调用 Game 的方法

Card::Card(std::string n, int a, Color c, std::map<Resource, int> co,
           std::vector<std::string> pre, std::string pro, CardEffect eff)
    : name(std::move(n)), age(a), color(c), cost(std::move(co)),
      chain_prerequisites(std::move(pre)), chain_provides(std::move(pro)),
      effect(std::move(eff)), is_face_up(false) {}

bool Card::can_build_free(const Player& player) const {
    for (const auto& pre : chain_prerequisites) {
        if (player.has_card(pre)) return true;
    }
    return false;
}

void Card::apply_effect(Player& player, Game& game) {
    if (effect) effect(player, game);
}

std::vector<std::unique_ptr<Card>> createAllCards() {
    std::vector<std::unique_ptr<Card>> cards;

    // 时代 I 卡牌 (49 张)
    // 棕色 (基本原料)
    cards.push_back(std::make_unique<Card>("Lumber Yard", 1, Color::BROWN, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::WOOD, 1); }));
    cards.push_back(std::make_unique<Card>("Ore Vein", 1, Color::BROWN, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::ORE, 1); }));
    cards.push_back(std::make_unique<Card>("Clay Pool", 1, Color::BROWN, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::BRICK, 1); }));
    cards.push_back(std::make_unique<Card>("Stone Pit", 1, Color::BROWN, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::STONE, 1); }));
    cards.push_back(std::make_unique<Card>("Timber Yard", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::WOOD, Resource::STONE}, 1); }));
    cards.push_back(std::make_unique<Card>("Clay Pit", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::BRICK, Resource::ORE}, 1); }));
    cards.push_back(std::make_unique<Card>("Excavation", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::STONE, Resource::BRICK}, 1); }));
    cards.push_back(std::make_unique<Card>("Forest Cave", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::WOOD, Resource::ORE}, 1); }));
    cards.push_back(std::make_unique<Card>("Tree Farm", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::WOOD, Resource::BRICK}, 1); }));
    cards.push_back(std::make_unique<Card>("Mine", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::ORE, Resource::STONE}, 1); }));

    // 灰色 (手工艺品)
    cards.push_back(std::make_unique<Card>("Loom", 1, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::CLOTH, 1); }));
    cards.push_back(std::make_unique<Card>("Glassworks", 1, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::GLASS, 1); }));
    cards.push_back(std::make_unique<Card>("Press", 1, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::PAPYRUS, 1); }));

    // 蓝色 (市政)
    cards.push_back(std::make_unique<Card>("Pawnshop", 1, Color::BLUE, {}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(3); }));
    cards.push_back(std::make_unique<Card>("Altar", 1, Color::BLUE, {}, {}, "moon", [&](Player& s, Game& g) { s.add_victory_points(2); }));  // 提供 moon 符号
    cards.push_back(std::make_unique<Card>("Baths", 1, Color::BLUE, {{Resource::STONE, 1}}, {}, "vase", [&](Player& s, Game& g) { s.add_victory_points(3); }));  // 提供 vase
    cards.push_back(std::make_unique<Card>("Theater", 1, Color::BLUE, {}, {}, "mask", [&](Player& s, Game& g) { s.add_victory_points(2); }));  // 提供 mask

    // 黄色 (商业)
    cards.push_back(std::make_unique<Card>("Tavern", 1, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.add_coins(5); }));
    cards.push_back(std::make_unique<Card>("East Trading Post", 1, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.reduce_trade_cost_left(1); }));  // 右邻折扣
    cards.push_back(std::make_unique<Card>("West Trading Post", 1, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.reduce_trade_cost_right(1); }));  // 左邻折扣
    cards.push_back(std::make_unique<Card>("Marketplace", 1, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.reduce_trade_cost_both(1); }));  // 两侧折扣

    // 红色 (军事)
    cards.push_back(std::make_unique<Card>("Stockade", 1, Color::RED, {{Resource::WOOD, 1}}, {}, "", [&](Player& s, Game& g) { s.add_shield(1); }));
    cards.push_back(std::make_unique<Card>("Barracks", 1, Color::RED, {{Resource::ORE, 1}}, {}, "", [&](Player& s, Game& g) { s.add_shield(1); }));
    cards.push_back(std::make_unique<Card>("Guard Tower", 1, Color::RED, {{Resource::BRICK, 1}}, {}, "", [&](Player& s, Game& g) { s.add_shield(1); }));

    // 灰色
    cards.push_back(std::make_unique<Card>("Loom", 1, Color::GREY, std::map<Resource, int>{}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_resource(Resource::CLOTH, 1); }));
    cards.push_back(std::make_unique<Card>("Glassworks", 1, Color::GREY, std::map<Resource, int>{}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_resource(Resource::GLASS, 1); }));
    cards.push_back(std::make_unique<Card>("Press", 1, Color::GREY, std::map<Resource, int>{}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_resource(Resource::PAPYRUS, 1); }));

    // 蓝色
    cards.push_back(std::make_unique<Card>("Pawnshop", 1, Color::BLUE, std::map<Resource, int>{}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_victory_points(3); }));
    cards.push_back(std::make_unique<Card>("Altar", 1, Color::BLUE, std::map<Resource, int>{}, std::vector<std::string>{}, "moon", [](Player& s, Game& g) { s.add_victory_points(2); }));

    // 黄色
    cards.push_back(std::make_unique<Card>("Tavern", 1, Color::YELLOW, std::map<Resource, int>{}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_coins(5); }));
    cards.push_back(std::make_unique<Card>("East Trading Post", 1, Color::YELLOW, std::map<Resource, int>{}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.reduce_trade_cost_right(1); }));

    // 红色
    cards.push_back(std::make_unique<Card>("Stockade", 1, Color::RED, std::map<Resource, int>{{Resource::WOOD, 1}}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_shield(1); }));

    // 绿色
    cards.push_back(std::make_unique<Card>("Apothecary", 1, Color::GREEN, std::map<Resource, int>{{Resource::CLOTH, 1}}, std::vector<std::string>{}, "COMPASS", [](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_COMPASS); g.check_science_victory(s); }));

    // --- 时代 II 卡牌 ---
    cards.push_back(std::make_unique<Card>("Sawmill", 2, Color::BROWN, std::map<Resource, int>{{Resource::COIN, 1}}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_resource(Resource::WOOD, 2); }));
    cards.push_back(std::make_unique<Card>("Aqueduct", 2, Color::BLUE, std::map<Resource, int>{{Resource::STONE, 3}}, std::vector<std::string>{"Baths"}, "", [](Player& s, Game& g) { s.add_victory_points(5); }));
    cards.push_back(std::make_unique<Card>("Vineyard", 2, Color::YELLOW, std::map<Resource, int>{}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_coins(s.count_own_brown()); })); // 简化了邻居逻辑

    // --- 时代 III 卡牌 ---
    cards.push_back(std::make_unique<Card>("Palace", 3, Color::BLUE, std::map<Resource, int>{{Resource::STONE, 1}, {Resource::WOOD, 1}, {Resource::ORE, 1}}, std::vector<std::string>{}, "", [](Player& s, Game& g) { s.add_victory_points(8); }));

    // 紫色 (行会)
    cards.push_back(std::make_unique<Card>("Workers Guild", 3, Color::PURPLE, std::map<Resource, int>{{Resource::ORE, 2}}, std::vector<std::string>{}, "", [](Player& s, Game& g) {
        // 这里的邻居统计建议通过 Game 获取对手
        // s.add_victory_points(g.getOpponent(s).count_own_brown());
    }));

    return cards;
}