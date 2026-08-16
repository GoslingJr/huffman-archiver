#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

// Узел двоичного дерева Хаффмана.
// Для листьев symbol содержит значащий байт, для внутренних узлов symbol не используется.
struct HuffmanNode {
    unsigned char symbol = 0;
    uint64_t frequency = 0;
    std::shared_ptr<HuffmanNode> left = nullptr;
    std::shared_ptr<HuffmanNode> right = nullptr;

    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

using HuffmanNodePtr = std::shared_ptr<HuffmanNode>;

// Таблица частот: индекс — значение байта (0..255), значение — сколько раз он встретился.
using FrequencyTable = std::array<uint64_t, 256>;

// Таблица префиксных кодов: символ -> строка из '0' и '1'.
using CodeTable = std::unordered_map<unsigned char, std::string>;

class HuffmanTree {
public:
    // Строит дерево Хаффмана по таблице частот.
    void build(const FrequencyTable& frequencies);

    // Обходит дерево и строит таблицу префиксных кодов.
    CodeTable buildCodeTable() const;

    HuffmanNodePtr getRoot() const { return root_; }

private:
    HuffmanNodePtr root_;

    void collectCodes(const HuffmanNodePtr& node, const std::string& code, CodeTable& table) const;
};
