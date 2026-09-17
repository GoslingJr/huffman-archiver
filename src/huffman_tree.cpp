#include "huffman_tree.h"
#include "bit_io.h"

#include <queue>
#include <stdexcept>
#include <vector>

void HuffmanTree::build(const std::array<uint64_t, 256>& frequencies) {
    // Компаратор для очереди с приоритетами: наверху должен быть узел с
    // наименьшей частотой (min-heap). При равных частотах сравниваем по
    // символу — это делает построение дерева детерминированным: одна и та
    // же таблица частот всегда даёт одно и то же дерево (важно, потому что
    // дерево заново строится при разжатии файла). Компаратор объявлен
    // локально в build(), а не отдельным типом снаружи класса, — снаружи
    // класса нет доступа к приватному типу Node.
    auto compare = [](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
        if (a->frequency != b->frequency) {
            return a->frequency > b->frequency;
        }
        return a->symbol > b->symbol;
    };

    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, decltype(compare)>
        queue(compare);

    for (int symbol = 0; symbol < 256; ++symbol) {
        if (frequencies[symbol] > 0) {
            auto node = std::make_shared<Node>();
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
        std::shared_ptr<Node> left = queue.top();
        queue.pop();
        std::shared_ptr<Node> right = queue.top();
        queue.pop();

        auto parent = std::make_shared<Node>();
        parent->frequency = left->frequency + right->frequency;
        parent->left = left;
        parent->right = right;

        queue.push(parent);
    }

    root_ = queue.top();
}

std::unordered_map<unsigned char, std::string> HuffmanTree::buildCodeTable() const {
    std::unordered_map<unsigned char, std::string> table;
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

void HuffmanTree::collectCodes(const std::shared_ptr<Node>& node, const std::string& code,
                                std::unordered_map<unsigned char, std::string>& table) const {
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

unsigned char HuffmanTree::decodeSymbol(BitReader& reader) const {
    if (root_ == nullptr) {
        throw std::runtime_error("Дерево Хаффмана не построено");
    }

    if (root_->isLeaf()) {
        // Особый случай: единственный уникальный символ во всём файле —
        // дерево без рёбер. При сжатии для него был записан один бит на
        // символ (см. buildCodeTable), поэтому здесь читаем и отбрасываем
        // ровно один бит, а возвращаем единственный существующий символ.
        bool bit;
        if (!reader.readBit(bit)) {
            throw std::runtime_error("Неожиданный конец сжатых данных");
        }
        return root_->symbol;
    }

    std::shared_ptr<Node> current = root_;
    while (!current->isLeaf()) {
        bool bit;
        if (!reader.readBit(bit)) {
            throw std::runtime_error("Неожиданный конец сжатых данных");
        }
        current = bit ? current->right : current->left;
    }
    return current->symbol;
}
