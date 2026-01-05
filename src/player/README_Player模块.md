# Player 模块使用说明

## 成员3职责：玩家逻辑与经济系统

本模块实现了 7 Wonders Duel 游戏中的玩家逻辑和经济系统，包括：
- 资源管理
- 金币管理
- 交易系统
- 连锁建造
- 军事和科技系统


## 核心功能说明

### 1. 玩家初始化
```cpp
// 创建人类玩家
Player player1("Alice", PlayerType::HUMAN);

// 创建AI玩家
Player player2("Bot", PlayerType::AI_RANDOM);
```

### 2. 资源管理
在《七大奇迹：对决》中，资源代表**永久产出能力**，而不是可消耗的库存。
```cpp
// 添加资源产出（建造褐色/灰色卡时调用）
player.add_resource(Resource::CLAY, 1);
player.add_resource_producing_card(Resource::CLAY);  // 注册产出符号，用于对手计算交易成本

// 添加二选一资源（如：林场提供 木头 或 石头）
player.add_wildcard_resource({Resource::WOOD, Resource::STONE});

// 添加贸易折扣（黄色卡效果：购买该资源固定为 1 金币）
player.add_trading_post(Resource::WOOD);

// 检查产出能力
int woodProduction = player.get_resource(Resource::WOOD);
```

### 3. 金币管理
金币是游戏中唯一的消耗性经济资源。
```cpp
// 获取金币
int coins = player.get_coins();

// 增加金币
player.add_coins(3);

// 消耗金币（建造或购买资源时调用）
if (player.spend_coins(5)) {
    // 成功消耗
}
```

### 4. 建造卡牌（使用 CostCalculator）

建造逻辑已完全移至 `CostCalculator`，它会自动处理连锁建造、资源产出、二选一资源和交易成本。

#### 4.1 检查并执行建造
```cpp
// 假设有一张卡牌对象 card
if (CostCalculator::canAffordWithTrade(player, opponent, card)) {
    // 执行建造：自动扣除金币（含卡牌成本和交易费）
    // 注意：资源是永久产出，不会被扣除
    if (CostCalculator::executeBuild(player, opponent, card)) {
        player.add_built_card(card.name, card.color);
        // 如果是资源卡，还需调用 player.add_resource 等
    }
}
```

#### 4.2 详细成本分析
```cpp
auto result = CostCalculator::calculateBuildCost(player, opponent, card);

if (result.isFreeByChain) {
    std::cout << "通过连锁建造免费！" << std::endl;
} else if (result.canBuild) {
    std::cout << "总金币成本: " << result.totalCoinCost << std::endl;
    // result.resourcesToBuy 包含了需要从对手购买的资源清单
}
```

### 5. 交易系统

#### 交易成本计算规则
1. 如果玩家拥有该资源的**贸易折扣**（黄色卡）：成本 = **1 金币**。
2. 否则：成本 = **2 + 对手拥有该类资源的产出符号数量**。

```cpp
// 内部逻辑示例
int cost = CostCalculator::calculateTradeCost(player, opponent, Resource::WOOD);
```

### 6. 军事系统
```cpp
// 添加军事标记（己方前进）
player.add_shield(2);

// 检查军事胜利
if (player.has_military_victory()) {
    std::cout << player.get_name() << " 军事获胜！" << std::endl;
}
```

### 7. 科技系统
```cpp
// 添加科技符号
player.add_science_symbol(Resource::SCIENCE_COMPASS);
player.add_science_symbol(Resource::SCIENCE_WHEEL);

// 检查科技胜利
if (player.has_science_victory()) {
    std::cout << player.get_name() << " 科技获胜！" << std::endl;
}
```

### 8. 胜利点数
```cpp
// 添加胜利点数（蓝卡、紫卡等）
player.add_victory_points(5);

// 计算最终得分（包括金币）
int finalScore = player.calculate_final_score();
// finalScore = victoryPoints + coins/3
```

## 完整建造流程示例

```cpp
void buildCardExample(Player& player, Player& opponent, const Card& card) {
    // 1. 检查连锁建造
    if (player.has_card(card.name)) { // 简化逻辑，实际应检查 chain_prerequisites
        std::cout << "连锁建造，免费！" << std::endl;
        player.add_built_card(card.name, card.color);
        
        // 注册资源产出卡（如果是褐色/灰色卡）
        if (card.color == Color::BROWN || card.color == Color::GREY) {
            // 假设卡牌产出木头
            player.add_resource_producing_card(Resource::WOOD);
        }
        
        return;
    }
    
    // 2. 检查是否能建造（考虑交易）
    if (!CostCalculator::canAffordWithTrade(player, opponent, card)) {
        std::cout << "资源/金币不足，无法建造！" << std::endl;
        return;
    }
    
    // 3. 执行建造
    if (CostCalculator::executeBuild(player, opponent, card)) {
        std::cout << "建造成功：" << card.name << std::endl;
        
        // 4. 添加已建造卡牌
        player.add_built_card(card.name, card.color);
        
        // 5. 注册资源产出卡
        if (card.color == Color::BROWN || card.color == Color::GREY) {
            // 根据卡牌效果添加资源
            player.add_resource_producing_card(Resource::WOOD);
        }
        
        // 6. 应用卡牌效果
        // if (card.effect) {
        //     card.effect(player, game);
        // }
    }
}
```

## 与其他模块的接口

### 与 Card 模块交互
```cpp
// Card 模块提供卡牌定义
Card card;
card.name = "Lumber Yard";
card.color = Color::BROWN;
card.cost = {}; // 免费
card.chain_provides = "WOOD_SYMBOL";

// Player 模块使用卡牌信息建造
player.add_built_card(card.name, card.color);
```

### 与 Game 模块交互
```cpp
// Game 模块管理两名玩家和回合流程
class Game {
    Player player1;
    Player player2;
    
    void playerTurn(Player& currentPlayer, Player& opponent) {
        // 玩家选择操作
        // 使用 CostCalculator 计算成本
        // 调用 Player 的方法执行操作
    }
};
```

### 与 View/Controller 模块交互
```cpp
// View 模块显示玩家状态
void displayPlayerStatus(const Player& player) {
    std::cout << "玩家: " << player.get_name() << std::endl;
    std::cout << "金币: " << player.get_coins() << std::endl;
    std::cout << "胜利点数: " << player.get_victory_points() << std::endl;
    std::cout << "已建造卡牌数: " << player.get_built_cards().size() << std::endl;
}

// Controller 模块处理用户输入
void handleBuildAction(Player& player, Player& opponent, const Card& card) {
    if (CostCalculator::canAffordWithTrade(player, opponent, card)) {
        CostCalculator::executeBuild(player, opponent, card);
        player.add_built_card(card.name, card.color);
    } else {
        std::cout << "无法建造此卡牌！" << std::endl;
    }
}
```

## 关键规则实现

### ✅ 已实现的规则
1. **初始金币7个** - `Player()` 构造函数
2. **交易成本计算** - `CostCalculator::calculateTradeCost()`
3. **连锁建造** - `Player::has_card()`
4. **资源管理** - `add_resource()`, `get_resource()`
5. **军事胜利** - `has_military_victory()`
6. **科技胜利** - `has_science_victory()`
7. **最终计分** - `calculate_final_score()` (VP + 金币/3)

### 📝 需要其他模块配合的规则
- 卡牌金字塔结构（成员2负责）
- 游戏主循环和回合管理（成员1负责）
- 用户界面和输入处理（成员4负责）

## 测试建议

### 单元测试示例
```cpp
void testTradeCost() {
    Player buyer("Alice");
    Player seller("Bob");
    
    // Bob 有 3 张木头卡
    seller.addResourceProducingCard(Resource::WOOD);
    seller.addResourceProducingCard(Resource::WOOD);
    seller.addResourceProducingCard(Resource::WOOD);
    
    // 计算交易成本
    int cost = CostCalculator::calculateTradeCost(buyer, seller, Resource::WOOD);
    
    assert(cost == 5);  // 2 + 3 = 5
    std::cout << "交易成本测试通过！" << std::endl;
}

void testChainBuilding() {
    Player player("Alice");
    
    // 建造前置建筑
    player.addBuiltCard("Lumber Yard", Color::BROWN);
    
    // 检查连锁建造
    std::vector<std::string> prereqs = {"Lumber Yard", "Quarry"};
    assert(player.canBuildFreeByChain(prereqs) == true);
    
    std::cout << "连锁建造测试通过！" << std::endl;
}
```

## 常见问题

### Q: 如何处理黄卡的特殊资源产出？
A: 在建造黄卡时，根据卡牌效果调用 `addResource()` 或 `addResourceProducingCard()`。

### Q: 科技胜利的"3对相同符号"如何实现？
A: 当前简化为收集6个不同符号。完整实现需要维护每种符号的数量，可以在 Player 类中添加 `std::map<Resource, int> scienceSymbolCounts`。

## 后续优化建议

1. **科技符号详细统计**：维护每种科技符号的数量，支持"3对相同符号"胜利条件
2. **奇迹建造**：添加 Wonder 相关的资源管理
3. **特殊卡牌效果**：扩展资源产出逻辑，支持黄卡的多种资源选择
4. **日志系统**：添加操作日志，方便调试和显示
5. **存档/读档**：支持游戏状态序列化


本模块（成员3）需要与以下模块协作：
- **成员1**：提供 Game 类接口，调用 Player 和 CostCalculator
- **成员2**：提供 Card 类定义，确保 Resource 枚举一致
- **成员4**：提供 View 和 Controller，展示玩家状态和处理输入


