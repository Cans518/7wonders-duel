#include "Board.h"
#include <cmath>

Board::Board() {
    conflictPawnPosition = 9; // 初始在中间 
    for(int i=0; i<4; i++) militaryTokens[i] = true; // 4个标记都在
}

bool Board::movePawn(int amount) {
    conflictPawnPosition += amount;
    
    // 简单的控制台输出，用于调试
    std::cout << "[Board] Conflict Pawn moved to " << conflictPawnPosition << std::endl;

    // 检查是否有人立即获胜 (到达 0 或 18)
    // 规则参考: 
    if (conflictPawnPosition <= 0) {
        std::cout << "[Board] Player 2 Capital has been reached!" << std::endl;
        return true; 
    }
    if (conflictPawnPosition >= 18) {
        std::cout << "[Board] Player 1 Capital has been reached!" << std::endl;
        return true;
    }
    
    // TODO: 在这里检查是否触发了 Looting (弃币) 
    // 这需要调用 Player 的扣钱接口，暂时略过
    return false;
}

int Board::getPawnPosition() const {
    return conflictPawnPosition;
}

int Board::getMilitaryPoints(int playerIndex) const {
    // 根据棋子位置计算分数，这里仅作示例逻辑 
    int dist = std::abs(conflictPawnPosition - 9);
    if (dist < 3) return 0;
    if (dist < 6) return 2;
    if (dist < 9) return 5;
    return 10;
}