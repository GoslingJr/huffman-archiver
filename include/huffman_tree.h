#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>

class BitReader;

/**
 * @brief Дерево Хаффмана.
 *
 * Строится по таблице частот символов. Умеет выдавать таблицу префиксных
 * кодов (для сжатия) и декодировать символы из битового потока (для
 * разжатия), не раскрывая наружу своё внутреннее устройство: структура
 * узла — деталь реализации и наружу не отдаётся.
 */
class HuffmanTree {
public:
    /**
     * @brief Строит дерево Хаффмана по таблице частот.
     * @param frequencies таблица частот: индекс — значение байта (0..255),
     *        значение — сколько раз он встретился.
     */
    void build(const std::array<uint64_t, 256>& frequencies);

    /**
     * @brief Обходит дерево и строит таблицу префиксных кодов.
     * @return таблица "символ -> строка из '0' и '1'"; пуста, если дерево
     *         не было построено (build ещё не вызывался или таблица частот
     *         была пустой).
     */
    std::unordered_map<unsigned char, std::string> buildCodeTable() const;

    /**
     * @brief Декодирует один символ, читая биты из reader и спускаясь по
     *        дереву от корня до листа.
     * @param reader источник битов (тот же поток, что был записан
     *        соответствующими кодами при сжатии).
     * @return декодированный символ.
     * @throws std::runtime_error если дерево не построено, либо поток
     *         данных закончился раньше, чем удалось дойти до листа
     *         (повреждённые или обрезанные сжатые данные).
     */
    unsigned char decodeSymbol(BitReader& reader) const;

    /// @return true, если дерево не построено (пустая таблица частот).
    bool empty() const { return root_ == nullptr; }

private:
    // Узел двоичного дерева Хаффмана — деталь реализации, наружу не отдаётся.
    // Для листьев symbol содержит значащий байт, для внутренних узлов
    // symbol не используется.
    struct Node {
        unsigned char symbol = 0;
        uint64_t frequency = 0;
        std::shared_ptr<Node> left = nullptr;
        std::shared_ptr<Node> right = nullptr;

        bool isLeaf() const { return left == nullptr && right == nullptr; }
    };

    std::shared_ptr<Node> root_;

    void collectCodes(const std::shared_ptr<Node>& node, const std::string& code,
                      std::unordered_map<unsigned char, std::string>& table) const;
};
