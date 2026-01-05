#include "Board.h"
#include "../player/Player.h"
#include <algorithm>

Board::Board() : pawn_position(9) {
    for(int i = 0; i < 4; ++i) military_tokens_active[i] = true;
}

bool Board::move_pawn(int amount, Player& p1, Player& p2) {
    pawn_position += amount;
    
    // 边界限制
    if (pawn_position < 0) pawn_position = 0;
    if (pawn_position > 18) pawn_position = 18;

    // --- 军事惩罚逻辑 (Looting Tokens) ---

    // 棋子向 P1 侧移动 (amount < 0)，检查 P1 侧标记
    if (amount < 0) {
        if (pawn_position <= 6 && military_tokens_active[0]) {
            p1.add_coins(-2); 
            military_tokens_active[0] = false;
            std::cout << "[Board] " << p1.get_name() << " lost 2 coins (Military Penalty)!\n";
        }
        if (pawn_position <= 3 && military_tokens_active[1]) {
            p1.add_coins(-5); 
            military_tokens_active[1] = false;
            std::cout << "[Board] " << p1.get_name() << " lost 5 coins (Military Penalty)!\n";
        }
    } 
    // 棋子向 P2 侧移动 (amount > 0)，检查 P2 侧标记
    else if (amount > 0) {
        if (pawn_position >= 12 && military_tokens_active[2]) {
            p2.add_coins(-2); 
            military_tokens_active[2] = false;
            std::cout << "[Board] " << p2.get_name() << " lost 2 coins (Military Penalty)!\n";
        }
        if (pawn_position >= 15 && military_tokens_active[3]) {
            p2.add_coins(-5); 
            military_tokens_active[3] = false;
            std::cout << "[Board] " << p2.get_name() << " lost 5 coins (Military Penalty)!\n";
        }
    }

    // 返回是否有人获胜
    return (pawn_position == 0 || pawn_position == 18);
}

int Board::get_military_vp(int player_index) const {
    // 计算距离：棋子距离自己半场的距离
    // P1 (idx 0) 得分区域在 10-18
    // P2 (idx 1) 得分区域在 0-8
    int distance = 0;
    if (player_index == 0) {
        distance = pawn_position - 9;
    } else {
        distance = 9 - pawn_position;
    }

    if (distance <= 0) return 0;   // 棋子在己方半场，不加分
    if (distance <= 2) return 2;   // 第一阶梯
    if (distance <= 5) return 5;   // 第二阶梯
    return 10;                     // 第三阶梯 (不含18，因为18直接获胜了)
}

void Board::setup_progress_tokens(const std::vector<ProgressToken>& tokens) {
    active_progress_tokens = tokens;
}

void Board::remove_progress_token(ProgressToken token) {
    auto it = std::find(active_progress_tokens.begin(), active_progress_tokens.end(), token);
    if (it != active_progress_tokens.end()) {
        active_progress_tokens.erase(it);
    }
}