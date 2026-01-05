#include "CardStructure.h"
#include <stdexcept>
#include <algorithm>

CardStructure::CardStructure(int age, std::vector<std::unique_ptr<Card>> deck) 
    : current_age(age), cards(std::move(deck)) {
    
    // 规则校验：对决版每时代使用 20 张牌（时代 III 包含 3 张公会卡共 23 张槽位）
    // 为了逻辑统一，此处按您之前要求的 20 张逻辑进行布局
    if (cards.size() != 20) {
        throw std::runtime_error("CardStructure Error: Deck must contain exactly 20 cards.");
    }

    setup_dependencies_and_faces(age);
}

void CardStructure::add_dependency(int supporter, int target) {
    unlocks[supporter].push_back(target);
    dependency_count[target]++;
}

void CardStructure::setup_dependencies_and_faces(int age) {
    unlocks.clear();
    dependency_count.clear();
    accessible.clear();

    if (age == 1) {
        // --- Age I: 正金字塔 (底部 6 -> 顶部 2) ---
        // 层级索引: L1(0-5), L2(6-10), L3(11-14), L4(15-17), L5(18-19)
        for (int i = 0; i <= 5; ++i) accessible.insert(i);

        // L1 -> L2
        add_dependency(0, 6); add_dependency(1, 6); add_dependency(1, 7); add_dependency(2, 7);
        add_dependency(2, 8); add_dependency(3, 8); add_dependency(3, 9); add_dependency(4, 9);
        add_dependency(4, 10); add_dependency(5, 10);
        // L2 -> L3
        add_dependency(6, 11); add_dependency(7, 11); add_dependency(7, 12); add_dependency(8, 12);
        add_dependency(8, 13); add_dependency(9, 13); add_dependency(9, 14); add_dependency(10, 14);
        // L3 -> L4
        add_dependency(11, 15); add_dependency(12, 15); add_dependency(12, 16); add_dependency(13, 16);
        add_dependency(13, 17); add_dependency(14, 17);
        // L4 -> L5
        add_dependency(15, 18); add_dependency(16, 18); add_dependency(16, 19); add_dependency(17, 19);

        // 初始可见性: L1, L3, L5 翻开; L2, L4 盖住
        for (int i = 0; i < 20; ++i) {
            if (i <= 5) cards[i]->is_face_up = true;        // L1
            else if (i <= 10) cards[i]->is_face_up = false; // L2
            else if (i <= 14) cards[i]->is_face_up = true;  // L3
            else if (i <= 17) cards[i]->is_face_up = false; // L4
            else cards[i]->is_face_up = true;               // L5
        }

    } else if (age == 2) {
        // --- Age II: 倒金字塔 (顶部 2 -> 底部 6) ---
        // 层级索引: L1(0-1), L2(2-4), L3(5-8), L4(9-13), L5(14-19)
        for (int i = 0; i <= 1; ++i) accessible.insert(i);

        add_dependency(0, 2); add_dependency(0, 3); add_dependency(1, 3); add_dependency(1, 4);
        add_dependency(2, 5); add_dependency(2, 6); add_dependency(3, 6); add_dependency(3, 7); add_dependency(4, 7); add_dependency(4, 8);
        add_dependency(5, 9); add_dependency(5, 10); add_dependency(6, 10); add_dependency(6, 11); add_dependency(7, 11); add_dependency(7, 12); add_dependency(8, 12); add_dependency(8, 13);
        add_dependency(9, 14); add_dependency(10, 14); add_dependency(10, 15); add_dependency(11, 15); add_dependency(11, 16); add_dependency(11, 17); add_dependency(12, 17); add_dependency(12, 18); add_dependency(13, 18); add_dependency(13, 19);

        // 初始可见性: 倒金字塔规则相反
        for (int i = 0; i < 20; ++i) {
            if (i <= 1) cards[i]->is_face_up = true;
            else if (i <= 4) cards[i]->is_face_up = false;
            else if (i <= 8) cards[i]->is_face_up = true;
            else if (i <= 13) cards[i]->is_face_up = false;
            else cards[i]->is_face_up = true;
        }

    } else if (age == 3) {
        // --- Age III: 括号形/环形布局 (20张简化版) ---
        for (int i = 0; i <= 1; ++i) accessible.insert(i);

        add_dependency(0, 2); add_dependency(0, 3); add_dependency(1, 3); add_dependency(1, 4); 
        add_dependency(2, 5); add_dependency(3, 6); add_dependency(3, 7); add_dependency(4, 8); 
        add_dependency(5, 9); add_dependency(6, 9); add_dependency(7, 10); add_dependency(8, 10); 
        add_dependency(9, 11); add_dependency(9, 12); add_dependency(10, 13); add_dependency(10, 14); 
        add_dependency(11, 15); add_dependency(12, 15); add_dependency(12, 16); 
        add_dependency(13, 16); add_dependency(13, 17); add_dependency(14, 17); 
        add_dependency(15, 18); add_dependency(16, 18); add_dependency(16, 19); add_dependency(17, 19); 

        for (int i = 0; i < 20; ++i) {
            if (i <= 1) cards[i]->is_face_up = true;
            else if (i <= 4) cards[i]->is_face_up = false;
            else if (i <= 8) cards[i]->is_face_up = true;
            else if (i <= 10) cards[i]->is_face_up = false;
            else if (i <= 14) cards[i]->is_face_up = true;
            else if (i <= 17) cards[i]->is_face_up = false;
            else cards[i]->is_face_up = true;
        }
    }
}

std::unique_ptr<Card> CardStructure::take_card(int pos) {
    // 1. 验证是否可拿取
    if (accessible.find(pos) == accessible.end()) {
        throw std::runtime_error("Logic Error: Card at position " + std::to_string(pos) + " is blocked!");
    }

    // 2. 转移所有权给调用者
    auto card = std::move(cards[pos]);
    accessible.erase(pos);

    // 3. 解锁上方卡牌逻辑
    if (unlocks.count(pos)) {
        for (int target : unlocks[pos]) {
            dependency_count[target]--;
            
            // 如果所有支撑物都消失了，则变为 accessible 并翻开
            if (dependency_count[target] == 0) {
                accessible.insert(target);
                if (cards[target]) {
                    cards[target]->is_face_up = true; // 自动翻开新露出的牌
                }
            }
        }
    }

    return card;
}

std::vector<int> CardStructure::get_accessible() const {
    return std::vector<int>(accessible.begin(), accessible.end());
}

bool CardStructure::is_empty() const {
    return accessible.empty();
}

const Card* CardStructure::get_card(int pos) const {
    if (pos < 0 || pos >= (int)cards.size()) return nullptr;
    return cards[pos].get();
}