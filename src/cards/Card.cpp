#include "Card.h"
#include "player/Player.h"
#include "core/Game.h"
#include <algorithm>

Card::Card(std::string n, int a, Color c) 
    : name(std::move(n)), age(a), color(c), is_face_up(false) {
    special_reward = SpecialReward();
}

bool Card::can_be_free(const Player& p) const {
    if (link_prerequisite == LinkSymbol::NONE) return false;
    return p.has_chain_symbol(link_prerequisite);
}

void Card::apply_effect(Player& p, Game& g) const {
    // 1. 基础数值
    if (victory_points > 0) p.add_victory_points(victory_points);
    if (shields > 0) g.move_pawn(shields);
    if (science_symbol >= Resource::COMPASS && science_symbol <= Resource::LAW) p.add_science_symbol(science_symbol);

    // 2. 连锁符号
    if (link_provides != LinkSymbol::NONE) p.add_chain_symbol(link_provides);

    // 3. 特殊收益
    if (special_reward.active) {
        int count = 0;
        Player* target_p = &p;
        if (special_reward.count_wonders) {
            count = p.count_wonder_stages();
            if (special_reward.count_both) count = std::max(count, g.get_opponent(p)->count_wonder_stages());
        } else {
            int own = p.get_card_count_by_color(special_reward.target_color);
            int opp = g.get_opponent(p)->get_card_count_by_color(special_reward.target_color);
            count = special_reward.count_both ? std::max(own, opp) : own;
        }
        p.add_coins(count * special_reward.coins_per_card);
    }

    // 4. 即时 Lambda
    if (immediate_func) immediate_func(p, g);
}

// --- 辅助创建器 ---
std::unique_ptr<Card> make_raw(std::string n, int a, Resource r, int gold = 0) {
    auto c = std::make_unique<Card>(n, a, Color::BROWN);
    if (gold > 0) c->cost = {{Resource::COIN, gold}};
    c->immediate_func = [r](Player& p, Game& g){ p.add_resource(r, 1); };
    return c;
}

std::vector<std::unique_ptr<Card>> createAllCards() {
    std::vector<std::unique_ptr<Card>> cards;

    // ======================== AGE I (23 Cards) ========================
    cards.push_back(make_raw("Lumber Yard", 1, Resource::WOOD));
    cards.push_back(make_raw("Logging Camp", 1, Resource::WOOD, 1));
    cards.push_back(make_raw("Clay Pool", 1, Resource::CLAY));
    cards.push_back(make_raw("Clay Pit", 1, Resource::CLAY, 1));
    cards.push_back(make_raw("Quarry", 1, Resource::STONE));
    cards.push_back(make_raw("Stone Pit", 1, Resource::STONE, 1));
    
    auto glass1 = std::make_unique<Card>("Glassworks", 1, Color::GREY);
    glass1->cost = {{Resource::COIN, 1}}; glass1->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::GLASS, 1); };
    cards.push_back(std::move(glass1));

    auto press1 = std::make_unique<Card>("Press", 1, Color::GREY);
    press1->cost = {{Resource::COIN, 1}}; press1->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::PAPYRUS, 1); };
    cards.push_back(std::move(press1));

    auto altar = std::make_unique<Card>("Altar", 1, Color::BLUE);
    altar->victory_points = 3; altar->link_provides = LinkSymbol::MOON;
    cards.push_back(std::move(altar));

    auto theater = std::make_unique<Card>("Theater", 1, Color::BLUE);
    theater->victory_points = 3; theater->link_provides = LinkSymbol::MASK;
    cards.push_back(std::move(theater));

    auto baths = std::make_unique<Card>("Baths", 1, Color::BLUE);
    baths->cost = {{Resource::STONE, 1}}; baths->victory_points = 3; baths->link_provides = LinkSymbol::DROP;
    cards.push_back(std::move(baths));

    auto workshop = std::make_unique<Card>("Workshop", 1, Color::GREEN);
    workshop->cost = {{Resource::PAPYRUS, 1}}; workshop->science_symbol = Resource::WHEEL; workshop->link_provides = LinkSymbol::TARGET;
    cards.push_back(std::move(workshop));

    auto apothecary = std::make_unique<Card>("Apothecary", 1, Color::GREEN);
    apothecary->cost = {{Resource::GLASS, 1}}; apothecary->science_symbol = Resource::COMPASS; apothecary->link_provides = LinkSymbol::LAMP;
    cards.push_back(std::move(apothecary));

    auto scriptorium = std::make_unique<Card>("Scriptorium", 1, Color::GREEN);
    scriptorium->cost = {{Resource::COIN, 2}}; scriptorium->science_symbol = Resource::TABLET; scriptorium->link_provides = LinkSymbol::BOOK;
    cards.push_back(std::move(scriptorium));

    auto pharmacist = std::make_unique<Card>("Pharmacist", 1, Color::GREEN);
    pharmacist->cost = {{Resource::COIN, 2}}; pharmacist->science_symbol = Resource::MORTAR; pharmacist->link_provides = LinkSymbol::GEAR;
    cards.push_back(std::move(pharmacist));

    auto tavern = std::make_unique<Card>("Tavern", 1, Color::YELLOW);
    tavern->immediate_func = [](Player& p, Game& g){ p.add_coins(4); }; tavern->link_provides = LinkSymbol::POT;
    cards.push_back(std::move(tavern));

    auto stone_res = std::make_unique<Card>("Stone Reserve", 1, Color::YELLOW);
    stone_res->cost = {{Resource::COIN, 3}}; stone_res->immediate_func = [](Player& p, Game& g){ p.set_fixed_trade_cost(Resource::STONE, 1); };
    cards.push_back(std::move(stone_res));

    auto clay_res = std::make_unique<Card>("Clay Reserve", 1, Color::YELLOW);
    clay_res->cost = {{Resource::COIN, 3}}; clay_res->immediate_func = [](Player& p, Game& g){ p.set_fixed_trade_cost(Resource::CLAY, 1); };
    cards.push_back(std::move(clay_res));

    auto wood_res = std::make_unique<Card>("Wood Reserve", 1, Color::YELLOW);
    wood_res->cost = {{Resource::COIN, 3}}; wood_res->immediate_func = [](Player& p, Game& g){ p.set_fixed_trade_cost(Resource::WOOD, 1); };
    cards.push_back(std::move(wood_res));

    for(int i=0; i<3; ++i) {
        auto m = std::make_unique<Card>("Garrison/Palisade/Guard", 1, Color::RED);
        m->shields = 1; m->link_provides = (i==0?LinkSymbol::SWORD:LinkSymbol::TOWER);
        cards.push_back(std::move(m));
    }

    // ======================== AGE II (23 Cards) ========================
    auto sawmill = std::make_unique<Card>("Sawmill", 2, Color::BROWN);
    sawmill->cost = {{Resource::COIN, 2}}; sawmill->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::WOOD, 2); };
    cards.push_back(std::move(sawmill));

    auto brickyard = std::make_unique<Card>("Brickyard", 2, Color::BROWN);
    brickyard->cost = {{Resource::COIN, 2}}; brickyard->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::CLAY, 2); };
    cards.push_back(std::move(brickyard));

    auto shelf = std::make_unique<Card>("Shelf Quarry", 2, Color::BROWN);
    shelf->cost = {{Resource::COIN, 2}}; shelf->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::STONE, 2); };
    cards.push_back(std::move(shelf));

    auto statue = std::make_unique<Card>("Statue", 2, Color::BLUE);
    statue->cost = {{Resource::CLAY, 2}}; statue->link_prerequisite = LinkSymbol::MASK; statue->victory_points = 4; statue->link_provides = LinkSymbol::STATUE;
    cards.push_back(std::move(statue));

    auto temple = std::make_unique<Card>("Temple", 2, Color::BLUE);
    temple->cost = {{Resource::WOOD, 1}, {Resource::PAPYRUS, 1}}; temple->link_prerequisite = LinkSymbol::MOON; temple->victory_points = 4; temple->link_provides = LinkSymbol::SUN;
    cards.push_back(std::move(temple));

    auto aqueduct = std::make_unique<Card>("Aqueduct", 2, Color::BLUE);
    aqueduct->cost = {{Resource::STONE, 3}}; aqueduct->link_prerequisite = LinkSymbol::DROP; aqueduct->victory_points = 5;
    cards.push_back(std::move(aqueduct));

    auto library = std::make_unique<Card>("Library", 2, Color::GREEN);
    library->cost = {{Resource::STONE, 1}, {Resource::WOOD, 1}, {Resource::GLASS, 1}}; library->link_prerequisite = LinkSymbol::BOOK; library->science_symbol = Resource::QUILL; library->victory_points = 2; library->link_provides = LinkSymbol::BOOK;
    cards.push_back(std::move(library));

    auto dispensary = std::make_unique<Card>("Dispensary", 2, Color::GREEN);
    dispensary->cost = {{Resource::CLAY, 2}, {Resource::GLASS, 1}}; dispensary->link_prerequisite = LinkSymbol::APOTHECARY; dispensary->science_symbol = Resource::MORTAR; dispensary->victory_points = 2; dispensary->link_provides = LinkSymbol::GEAR;
    cards.push_back(std::move(dispensary));

    auto forum = std::make_unique<Card>("Forum", 2, Color::YELLOW);
    forum->cost = {{Resource::CLAY, 1}, {Resource::COIN, 3}}; forum->immediate_func = [](Player& p, Game& g){ p.add_resource_choice({Resource::GLASS, Resource::PAPYRUS}); }; forum->link_provides = LinkSymbol::BARREL;
    cards.push_back(std::move(forum));

    auto brewery = std::make_unique<Card>("Brewery", 2, Color::YELLOW);
    brewery->link_prerequisite = LinkSymbol::POT; brewery->immediate_func = [](Player& p, Game& g){ p.add_coins(6); };
    cards.push_back(std::move(brewery));

    auto walls = std::make_unique<Card>("Walls", 2, Color::RED);
    walls->cost = {{Resource::STONE, 2}}; walls->shields = 2;
    cards.push_back(std::move(walls));

    // ... 补全其余 Age II 卡牌 (Barracks, Archery Range, Parade Ground, Laboratory, School, Customs House 等)
    for(int i=0; i<12; ++i) cards.push_back(std::make_unique<Card>("Age2_Fill_Card", 2, Color::RED));

    // ======================== AGE III (20 Cards) ========================
    auto palace = std::make_unique<Card>("Palace", 3, Color::BLUE);
    palace->cost = {{Resource::STONE, 2}, {Resource::CLAY, 2}, {Resource::WOOD, 2}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}};
    palace->victory_points = 8; palace->link_prerequisite = LinkSymbol::SUN;
    cards.push_back(std::move(palace));

    auto gardens = std::make_unique<Card>("Gardens", 3, Color::BLUE);
    gardens->cost = {{Resource::CLAY, 2}, {Resource::WOOD, 2}};
    gardens->victory_points = 6; gardens->link_prerequisite = LinkSymbol::STATUE;
    cards.push_back(std::move(gardens));

    auto arsenal = std::make_unique<Card>("Arsenal", 3, Color::RED);
    arsenal->cost = {{Resource::CLAY, 3}, {Resource::WOOD, 2}}; arsenal->shields = 3;
    cards.push_back(std::move(arsenal));

    auto arena = std::make_unique<Card>("Arena", 3, Color::YELLOW);
    arena->cost = {{Resource::CLAY, 1}, {Resource::STONE, 1}, {Resource::WOOD, 1}};
    arena->special_reward = Card::SpecialReward(true, Color::YELLOW, 2, 0, true); arena->victory_points = 3;
    cards.push_back(std::move(arena));

    // ... 补全其余 Age III 卡牌 (Senate, Town Hall, Observatory, Academy, University, Fortifications, Circus, Siege Workshop 等)
    for(int i=0; i<16; ++i) cards.push_back(std::make_unique<Card>("Age3_Fill_Card", 3, Color::BLUE));

    // ======================== GUILDS (7 Cards) ========================
    auto builders = std::make_unique<Card>("Builders Guild", 3, Color::PURPLE);
    builders->special_reward = Card::SpecialReward(true, Color::PURPLE, 0, 2, true, true);
    cards.push_back(std::move(builders));

    auto scientists = std::make_unique<Card>("Scientists Guild", 3, Color::PURPLE);
    scientists->special_reward = Card::SpecialReward(true, Color::GREEN, 1, 1, false, true);
    cards.push_back(std::move(scientists));

    auto tacticians = std::make_unique<Card>("Tacticians Guild", 3, Color::PURPLE);
    tacticians->special_reward = Card::SpecialReward(true, Color::RED, 1, 1, false, true);
    cards.push_back(std::move(tacticians));

    auto merchants = std::make_unique<Card>("Merchants Guild", 3, Color::PURPLE);
    merchants->special_reward = Card::SpecialReward(true, Color::YELLOW, 1, 1, false, true);
    cards.push_back(std::move(merchants));

    // 补全剩余公会 (Shipowners, Moneylenders, Magistrates)
    for(int i=0; i<3; ++i) cards.push_back(std::make_unique<Card>("Other Guild", 3, Color::PURPLE));

    return cards;
}