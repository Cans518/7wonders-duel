#pragma once
#include <string>
#include <vector>

// 模拟 Member 3 的 Player 类
class Player {
    std::string name;
    int coins;
public:
    Player(std::string n) : name(n), coins(0) {}
    void addCoins(int n) { coins += n; }
    std::string getName() { return name; }
    // 模拟科技符号检查
    std::vector<int> getScienceSymbols() { return {}; } 
};

// 模拟 Member 2 的 CardStructure 类
class CardStructure {
public:
    static void loadAge(int age) {}
    bool isEmpty() { return false; }
};