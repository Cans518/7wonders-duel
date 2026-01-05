#include "Wonder.h"
#include "player/Player.h"
#include "core/Game.h"

Wonder::Wonder(std::string n, std::map<Resource, int> co, WonderEffect eff)
    : name(std::move(n)), cost(std::move(co)), effect(std::move(eff)), 
      victory_points(0), shields(0), is_built(false) {}

std::vector<Wonder> createAllWonders() {
    std::vector<Wonder> wonders;

    // --- The Appian Way (阿皮亚道) ---
    // 规则：自己拿3金，对手丢3金，再动一回合，3分
    auto appian = Wonder("The Appian Way", {{Resource::STONE, 2}, {Resource::CLAY, 1}, {Resource::WOOD, 1}},
        [](Player& s, Player& o, Game& g) { 
            s.add_coins(3); 
            o.add_coins(-3); 
            g.set_extra_turn(true); 
        });
    appian.victory_points = 3;
    wonders.push_back(appian);

    // --- Circus Maximus (大竞技场) ---
    // 规则：拆掉对手一张灰卡，1盾，3分
    auto circus = Wonder("Circus Maximus", {{Resource::STONE, 2}, {Resource::GLASS, 1}},
        [](Player& s, Player& o, Game& g) { 
            o.destroy_card_by_color(Color::GREY); 
        });
    circus.victory_points = 3;
    circus.shields = 1;
    wonders.push_back(circus);

    // --- The Colossus (罗德岛巨像) ---
    // 规则：2盾，3分
    auto colossus = Wonder("The Colossus", {{Resource::CLAY, 3}, {Resource::GLASS, 1}});
    colossus.victory_points = 3;
    colossus.shields = 2;
    wonders.push_back(colossus);

    // --- The Great Library (大图书馆) ---
    // 规则：从盒子里抽3个进步标记选1个，4分
    auto library = Wonder("The Great Library", {{Resource::WOOD, 3}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}},
        [](Player& s, Player& o, Game& g) { 
            g.trigger_progress_token_selection(s, 3); 
        });
    library.victory_points = 4;
    wonders.push_back(library);

    // --- The Great Lighthouse (亚历山大灯塔) ---
    // 规则：每回合产出 1木/1泥/1石，4分
    auto lighthouse = Wonder("The Great Lighthouse", {{Resource::STONE, 2}, {Resource::WOOD, 1}, {Resource::PAPYRUS, 1}},
        [](Player& s, Player& o, Game& g) { 
            s.add_resource_choice({Resource::WOOD, Resource::CLAY, Resource::STONE}); 
        });
    lighthouse.victory_points = 4;
    wonders.push_back(lighthouse);

    // --- The Hanging Gardens (空中花园) ---
    // 规则：+6金，再动一回合，3分
    auto hanging = Wonder("The Hanging Gardens", {{Resource::WOOD, 2}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}},
        [](Player& s, Player& o, Game& g) { 
            s.add_coins(6); 
            g.set_extra_turn(true); 
        });
    hanging.victory_points = 3;
    wonders.push_back(hanging);

    // --- The Mausoleum (摩索拉斯王陵墓) ---
    // 规则：从弃牌堆免费建一张卡，2分
    auto mausoleum = Wonder("The Mausoleum", {{Resource::CLAY, 2}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}},
        [](Player& s, Player& o, Game& g) { 
            g.trigger_build_from_discard(s); 
        });
    mausoleum.victory_points = 2;
    wonders.push_back(mausoleum);

    // --- Piraeus (比雷埃夫斯港) ---
    // 规则：每回合产出 1玻/1纸，再动一回合，2分
    auto piraeus = Wonder("Piraeus", {{Resource::WOOD, 2}, {Resource::CLAY, 1}, {Resource::STONE, 1}},
        [](Player& s, Player& o, Game& g) { 
            s.add_resource_choice({Resource::GLASS, Resource::PAPYRUS}); 
            g.set_extra_turn(true); 
        });
    piraeus.victory_points = 2;
    wonders.push_back(piraeus);

    // --- The Pyramids (胡夫金字塔) ---
    // 规则：直接获得 9分
    auto pyramids = Wonder("The Pyramids", {{Resource::STONE, 3}, {Resource::PAPYRUS, 1}});
    pyramids.victory_points = 9;
    wonders.push_back(pyramids);

    // --- The Sphinx (狮身人面像) ---
    // 规则：再动一回合，6分
    auto sphinx = Wonder("The Sphinx", {{Resource::STONE, 1}, {Resource::CLAY, 1}, {Resource::GLASS, 2}},
        [](Player& s, Player& o, Game& g) { 
            g.set_extra_turn(true); 
        });
    sphinx.victory_points = 6;
    wonders.push_back(sphinx);

    // --- The Statue of Zeus (宙斯神像) ---
    // 规则：拆掉对手一张棕卡，1盾，3分
    auto zeus = Wonder("The Statue of Zeus", {{Resource::CLAY, 1}, {Resource::STONE, 1}, {Resource::PAPYRUS, 2}},
        [](Player& s, Player& o, Game& g) { 
            o.destroy_card_by_color(Color::BROWN); 
        });
    zeus.victory_points = 3;
    zeus.shields = 1;
    wonders.push_back(zeus);

    // --- The Temple of Artemis (阿尔忒弥斯神庙) ---
    // 规则：立即获得 12金，再动一回合
    auto artemis = Wonder("The Temple of Artemis", {{Resource::GLASS, 1}, {Resource::PAPYRUS, 1}, {Resource::STONE, 1}, {Resource::WOOD, 1}},
        [](Player& s, Player& o, Game& g) { 
            s.add_coins(12); 
            g.set_extra_turn(true); 
        });
    artemis.victory_points = 0; 
    wonders.push_back(artemis);

    return wonders;
}