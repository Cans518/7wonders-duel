#ifndef CARD_STRUCTURE_H
#define CARD_STRUCTURE_H

#include "Card.h"
#include <vector>
#include <map>
#include <set>
#include <memory>

class CardStructure {
private:
    std::vector<std::unique_ptr<Card>> cards;
    std::map<int, std::vector<int>> unlocks;
    std::map<int, int> dependency_count;
    std::set<int> accessible;
    int current_age;

    // --- 重点修改：确保函数名与 .cpp 一致 ---
    void add_dependency(int supporter, int target); 
    void setup_dependencies_and_faces(int age); // 之前这里可能叫 setup_layout

public:
    CardStructure(int age, std::vector<std::unique_ptr<Card>> deck);
    std::vector<int> get_accessible() const;
    std::unique_ptr<Card> take_card(int pos);
    bool is_empty() const;
    const Card* get_card(int pos) const; 
    int get_age() const { return current_age; }
};

#endif