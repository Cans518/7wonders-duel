#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>
#include "Types.h" // 必须包含，以识别 ProgressToken 枚举

// 前向声明，避免循环引用
class Player;

/**
 * Board 类：管理军事冲突条和场上的科技标记
 */
class Board {
private:
    int pawn_position; 
    
    // 军事惩罚标记（Looting Tokens）
    // [0]: P1侧 2元 (pos 6), [1]: P1侧 5元 (pos 3)
    // [2]: P2侧 2元 (pos 12), [3]: P2侧 5元 (pos 15)
    bool military_tokens_active[4]; 

    // 场上公开的科技标记
    std::vector<ProgressToken> active_progress_tokens;

public:
    Board();
    
    /**
     * 移动冲突棋子
     * @param amount 步数（P1增加则为正，P2增加则为负）
     * @return true 如果达成军事压制（到达 0 或 18）
     */
    bool move_pawn(int amount, Player& p1, Player& p2);
    
    int get_pawn_position() const { return pawn_position; }
    
    // 获取玩家当前的军事分数（基于棋子位置）
    int get_military_vp(int player_index) const;

    // --- 科技标记管理 ---
    void setup_progress_tokens(const std::vector<ProgressToken>& tokens);
    void remove_progress_token(ProgressToken token);
    const std::vector<ProgressToken>& get_active_progress_tokens() const { return active_progress_tokens; }
};

#endif