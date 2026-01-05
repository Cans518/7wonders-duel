#pragma once

enum class Color { BROWN, GREY, BLUE, YELLOW, RED, GREEN, PURPLE };

enum class Resource { 
    WOOD, CLAY, STONE, GLASS, PAPYRUS, COIN, VP,
    // 图片 2：科技符号精确对齐
    COMPASS,  // 罗盘
    WHEEL,    // 齿轮/轮子
    TABLET,   // 石板
    MORTAR,   // 研钵
    QUILL,    // 羽毛笔
    ARCH,     // 拱门
    LAW       // 法律 (仅通过进步板块)
};

// 图片 1：连锁符号 (Chains) 完全对齐
// src/Types.h 

enum class LinkSymbol {
    NONE,
    // --- 军事类 ---
    SWORD,      // 剑 (Garrison -> Barracks)
    TOWER,      // 塔 (Guard Tower -> Training Ground -> Circus)
    HORSESHOE,  // 马蹄铁 (Stable -> Horse Breeders) - 重点检查这一行！
    TARGET,     // 靶子 (Workshop -> Archery Range -> Siege Workshop)
    HELMET,     // 头盔 (Barracks -> Parade Ground -> Circus)

    // --- 科技类 ---
    BOOK,       // 书 (Scriptorium -> Library -> Senate)
    GEAR,       // 齿轮 (Workshop -> Laboratory -> Observatory)
    LAMP,       // 灯 (Apothecary -> Dispensary -> University)
    APOTHECARY, // 药瓶 (Apothecary -> Dispensary)

    // --- 市政类 ---
    MASK,       // 面具 (Theater -> Statue -> Gardens)
    MOON,       // 月亮 (Altar -> Temple -> Pantheon)
    SUN,        // 太阳 (Temple -> Palace)
    DROP,       // 水滴 (Baths -> Aqueduct)
    COLUMN,     // 柱子 (Courthouse -> 奇迹)
    VASE,       // 花瓶 (Baths -> Rostrum)
    STATUE,     // 雕像 (Statue -> Gardens)
    
    // --- 商业类 ---
    POT,        // 壶 (Tavern -> Brewery)
    BARREL,     // 桶 (Forum -> Haven)
    VESSEL,     // 容器 (Caravansery -> Lighthouse)
    
    // --- 时代 III 特有 ---
    LION,       // 狮子 (Rostrum -> Arena)
    CAPITOL     // 议事堂 (Senate -> Palace)
};

enum class PlayerType { HUMAN, AI_RANDOM };
enum class ProgressToken { AGRICULTURE, ARCHITECTURE, ECONOMY, LAW, MASONRY, MATHEMATICS, PHILOSOPHY, STRATEGY, THEOLOGY, URBANISM };