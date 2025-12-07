#pragma once
#include <vector>
#include <iostream>

class Board {
private:
    // 冲突棋子的位置。
    // 假设 0 是左边玩家(P1)的首都，18 是右边玩家(P2)的首都，9 是初始中间位置。
    // 规则参考: 
    int conflictPawnPosition; 
    
    // 军事标记（当棋子跨过某些区域时触发弃币）。
    // 这里简单用 bool 表示是否还在。规则参考: 
    bool militaryTokens[4]; 

    // 场上的科技标记（Progress Tokens）。规则参考: 
    // 假设用简单的 int ID 或 enum 代表不同的科技
    std::vector<int> progressTokens;

public:
    Board();
    
    // 移动棋子。amount > 0 向右(P2)移动，amount < 0 向左(P1)移动
    // 返回 true 如果导致某方立即获胜（到达首都）
    // 规则参考: 
    bool movePawn(int amount);
    
    int getPawnPosition() const;
    
    // 获取当前的军事分数（游戏结束时用）
    // 规则参考: 
    int getMilitaryPoints(int playerIndex) const;

    // TODO: 添加管理科技标记的方法
};