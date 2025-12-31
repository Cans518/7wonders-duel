#include "Wonder.h"
#include "../core/Game.h"

// 使用 std::move 提高效率//假设你最终决定使用 STONE 和 CLAY
Wonder::Wonder(std::string n, std::map<Resource, int> co, WonderEffect eff)
    : name(std::move(n)), cost(std::move(co)), effect(std::move(eff)) {}

std::vector<Wonder> createAllWonders() {
    std::vector<Wonder> wonders;

    // The Appian Way
    wonders.emplace_back("The Appian Way",
        std::map<Resource, int>{{Resource::STONE, 2}, {Resource::CLAY, 1}, {Resource::WOOD, 1}},
        [](Player& s, Player& o, Game& g) {
            o.addCoins(-3);
            g.setExtraTurn(true);
            s.addVictoryPoints(3);
        });

    // Circus Maximus
    wonders.emplace_back("Circus Maximus",
        std::map<Resource, int>{{Resource::STONE, 2}, {Resource::CLAY, 1}},
        [](Player& s, Player& o, Game& g) {
            o.destroyCardByColor(Color::GREY); // 需要在Player实现此方法
            g.movePawn(1);
            s.addVictoryPoints(3);
        });

    // The Colossus
    wonders.emplace_back("The Colossus",
        std::map<Resource, int>{{Resource::CLAY, 3}, {Resource::WOOD, 2}},
        [](Player& s, Player& o, Game& g) {
            g.movePawn(2);
            s.addVictoryPoints(3);
        });

    // The Great Library
    wonders.emplace_back("The Great Library",
        std::map<Resource, int>{{Resource::STONE, 2}, {Resource::WOOD, 2}},
        [](Player& s, Player& o, Game& g) {
            // 逻辑建议：触发游戏流程让玩家从3个标记中选一个
            g.triggerProgressTokenSelection(s, 3);
            s.addVictoryPoints(4);
        });

    // The Great Lighthouse
    wonders.emplace_back("The Great Lighthouse",
        std::map<Resource, int>{{Resource::STONE, 2}, {Resource::WOOD, 1}},
        [](Player& s, Player& o, Game& g) {
            s.add_resource_choice({Resource::STONE, Resource::CLAY, Resource::WOOD});
            s.addVictoryPoints(4);
        });

    // The Hanging Gardens
    wonders.emplace_back("The Hanging Gardens",
        std::map<Resource, int>{{Resource::STONE, 1}, {Resource::CLAY, 2}},
        [](Player& s, Player& o, Game& g) {
            s.addCoins(6);
            g.setExtraTurn(true);
            s.addVictoryPoints(3);
        });

    // The Mausoleum
    wonders.emplace_back("The Mausoleum",
        std::map<Resource, int>{{Resource::CLAY, 2}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}},
        [](Player& s, Player& o, Game& g) {
            g.triggerBuildFromDiscard(s); // 需要在Game实现从弃牌堆建造逻辑
            s.addVictoryPoints(2);
        });

    // Piraeus
    wonders.emplace_back("Piraeus",
        std::map<Resource, int>{{Resource::WOOD, 2}, {Resource::PAPYRUS, 1}},
        [](Player& s, Player& o, Game& g) {
            s.add_resource_choice({Resource::GLASS, Resource::PAPYRUS});
            g.setExtraTurn(true);
            s.addVictoryPoints(2);
        });

    // The Pyramids
    wonders.emplace_back("The Pyramids",
        std::map<Resource, int>{{Resource::STONE, 3}, {Resource::WOOD, 2}},
        [](Player& s, Player& o, Game& g) {
            s.addVictoryPoints(9);
        });

    // The Sphinx
    wonders.emplace_back("The Sphinx",
        std::map<Resource, int>{{Resource::CLAY, 1}, {Resource::GLASS, 1}},
        [](Player& s, Player& o, Game& g) {
            g.setExtraTurn(true);
            s.addVictoryPoints(6);
        });

    // The Statue of Zeus
    wonders.emplace_back("The Statue of Zeus",
        std::map<Resource, int>{{Resource::WOOD, 2}, {Resource::PAPYRUS, 1}},
        [](Player& s, Player& o, Game& g) {
            o.destroyCardByColor(Color::BROWN);
            g.movePawn(1);
            s.addVictoryPoints(3);
        });

    // The Temple of Artemis
    wonders.emplace_back("The Temple of Artemis",
        std::map<Resource, int>{{Resource::STONE, 1}, {Resource::WOOD, 1}, {Resource::PAPYRUS, 1}},
        [](Player& s, Player& o, Game& g) {
            s.addCoins(12);
            g.setExtraTurn(true);
        });

    return wonders;
}//如果你的 Resource 定义里是 BRICK 而不是 CLAY，请全局替换。否则 Resource::CLAY 会报 has no member named 'CLAY' 错误。