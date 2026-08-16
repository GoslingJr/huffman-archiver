#include "huffman_tree.h"
#include <queue>
#include <vector>

namespace {

// Компаратор для очереди с приоритетами: наверху должен быть узел с
// наименьшей частотой (min-heap). При равных частотах сравниваем по
// символу — это делает построение дерева детерминированным: одна и та же
// таблица частот всегда даёт одно и то же дерево (важно, потому что дерево
// заново строится при разжатии файла).
struct Compare {
    bool operator()(const HuffmanNodePtr& a, const HuffmanNodePtr& b) const {
        if (a->frequency != b->frequency) {
            return a->frequency > b->frequency;
        }
        return a->symbol > b->symbol;
    }
};

}  // namespace

void HuffmanTree::build(const FrequencyTable& frequencies) {
    std::priority_queue<HuffmanNodePtr, std::vector<HuffmanNodePtr>, Compare> queue;

    for (int symbol = 0; symbol < 256; ++symbol) {
        if (frequencies[symbol] > 0) {
            auto node = std::make_shared<HuffmanNode>();
            node->symbol = static_cast<unsigned char>(symbol);
            node->frequency = frequencies[symbol];
            queue.push(node);
        }
    }

    if (queue.empty()) {
        root_ = nullptr;
        return;
    }

    // Пока в очереди больше одного дерева — берём два самых редких узла
    // и склеиваем их в один новый узел с суммарной частотой.
    while (queue.size() > 1) {
        HuffmanNodePtr left = queue.top();
        queue.pop();
        HuffmanNodePtr right = queue.top();
        queue.pop();

        auto parent = std::make_shared<HuffmanNode>();
        parent->frequency = left->frequency + right->frequency;
        parent->left = left;
        parent->right = right;

        queue.push(parent);
    }

    root_ = queue.top();
}

CodeTable HuffmanTree::buildCodeTable() const {
    CodeTable table;
    if (root_ == nullptr) {
        return table;
    }

    if (root_->isLeaf()) {
        // Особый случай: во всём файле встречается только один уникальный
        // символ. Дерево состоит из одного узла без рёбер, поэтому обычный
        // обход не даст кода — присваиваем такому символу код "0" вручную.
        table[root_->symbol] = "0";
        return table;
    }

    collectCodes(root_, "", table);
    return table;
}

void HuffmanTree::collectCodes(const HuffmanNodePtr& node, const std::string& code,
                                CodeTable& table) const {
    if (node->isLeaf()) {
        table[node->symbol] = code;
        return;
    }
    if (node->left) {
        collectCodes(node->left, code + "0", table);
    }
    if (node->right) {
        collectCodes(node->right, code + "1", table);
    }
}
