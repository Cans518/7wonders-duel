#ifndef PLAYER_H
#define PLAYER_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <set>

// 前向声明
class Card;

// 直接使用 Card.h 中的 Resource 和 Color 枚举
enum class Resource { 
    WOOD, BRICK, ORE, STONE,           // 褐色资源
    GLASS, CLOTH, PAPYRUS,             // 灰色资源
    SHIELD,                             // 军事
    SCIENCE_COMPASS, SCIENCE_GEAR, SCIENCE_TABLET,  // 科技符号
    COIN, VP                            // 金币和胜利点
};

enum class Color { BROWN, GREY, BLUE, YELLOW, RED, GREEN, PURPLE };

// 玩家类型枚举
enum class PlayerType {
    HUMAN,      // 人类玩家
    AI_RANDOM   // 随机AI（简单AI实现）
};

/**
 * Player 类：管理玩家的资源、金币、已建造卡牌等状态
 * 核心功能：
 * 1. 资源管理：木头、砖块、矿石、石头、玻璃、布料、纸莎草
 * 2. 金币管理：初始7个金币，用于购买资源和卡牌
 * 3. 交易系统：购买资源费用 = 2 + 对手拥有该类褐色/灰色卡数量
 * 4. 连锁建造：拥有前置建筑可以免费建造某些卡牌
 * 5. 军事和科技：军事标记和科技符号收集
 */
class Player {
private:
    std::string name;                                   // 玩家姓名
    PlayerType type;                                    // 玩家类型（人类/AI）
    std::map<Resource, int> resources;                  // 玩家的资源产出能力（永久）
    int coins;                                          // 玩家金币数量
    std::vector<std::string> builtCards;                // 已建造的卡牌名称列表
    std::map<Resource, int> resourceProducingCards;     // 褐色/灰色卡提供的产出符号（用于对手交易成本）
    std::map<Color, int> cardsByColor;                  // 按颜色统计的卡牌数量
    std::set<Resource> tradingPosts;                    // 拥有贸易折扣的资源（交易费固定为1）
    std::vector<std::set<Resource>> wildcardResources;  // 二选一资源产出（如 {WOOD, STONE}）
    int militaryTokens;                                 // 军事标记
    int victoryPoints;                                  // 胜利点数
    std::set<Resource> scienceSymbols;                  // 收集的科技符号

public:
    // 构造函数：初始化玩家
    Player(const std::string& playerName = "Player", PlayerType playerType = PlayerType::HUMAN);

    // 玩家信息
    
    std::string getName() const { return name; }
    void setName(const std::string& playerName) { name = playerName; }
    
    PlayerType getType() const { return type; }
    void setType(PlayerType playerType) { type = playerType; }
    
    bool isHuman() const { return type == PlayerType::HUMAN; }
    bool isAI() const { return type == PlayerType::AI_RANDOM; }



    // 资源管理
    
    //添加资源产出：增加玩家的永久产出能力
    void addResource(Resource resource, int amount);

    //获取资源产出：返回玩家某种资源的总产出
    int getResource(Resource resource) const;

    //检查是否拥有贸易折扣
    bool hasTradingPost(Resource resource) const;

    //添加贸易折扣
    void addTradingPost(Resource resource);

    // 添加二选一资源产出
    void addWildcardResource(const std::set<Resource>& options);

    //获取所有二选一资源
    const std::vector<std::set<Resource>>& getWildcardResources() const { return wildcardResources; }

    //检查是否拥有足够资源
    bool hasEnoughResource(Resource resource, int amount) const;



    //金币管理
    
    //获取金币数量
    int getCoins() const;

    //消耗金币：扣除指定数量的金币
    bool spendCoins(int amount);

    //增加金币
    void addCoins(int amount);



    //卡牌管理
    
    // 添加已建造的卡牌：玩家建造卡牌后调用
    void addBuiltCard(const std::string& cardName, Color cardColor);

    //检查是否拥有指定建筑：用于连锁建造判断
    bool hasBuilding(const std::string& cardName) const;

    //获取已建造的卡牌列表
    const std::vector<std::string>& getBuiltCards() const;

    //获取某颜色卡牌的数量
    int getCardCountByColor(Color color) const;



    //资源产出卡统计（用于交易成本计算
    
    //注册资源产出卡：当玩家建造褐色/灰色卡时调用
    void addResourceProducingCard(Resource resource);

    //获取某类资源的产出卡数量：用于对手计算交易成本
    int getResourceProducingCardCount(Resource resource) const;



    // 军事 
    
    // 添加军事标记
    void addMilitaryTokens(int amount);

    //获取军事标记数量
    int getMilitaryTokens() const;

    //检查是否军事获胜（军事标记达到阈值）
    bool hasMilitaryVictory() const;




    
    //科技 
    
    // 添加科技符号
    void addScienceSymbol(Resource symbol);

    //获取收集的科技符号集合
    const std::set<Resource>& getScienceSymbols() const;

    //检查是否科技获胜（收集6个不同科技符号或3对相同科技符号）
    bool hasScienceVictory() const;




    //  胜利点数 
    
    //添加胜利点数
    void addVictoryPoints(int amount);

    //获取胜利点数
    int getVictoryPoints() const;

    // 计算最终得分（包括金币）
    int calculateFinalScore() const;


    
    // --- 新增：获取唯一科技符号数量 ---
    int getUniqueScienceCount() const { 
        return scienceSymbols.size(); 
    }

    // --- 新增：统计黄色卡牌数量（用于弃牌金币计算） ---
    int countYellowCards() const {
        return getCardCountByColor(Color::YELLOW);
    }
    
    // --- 新增：强制扣钱（用于奇迹效果或军事惩罚） ---
    void loseCoins(int amount) {
        coins = (coins > amount) ? (coins - amount) : 0;
    }
};

#endif // PLAYER_H
