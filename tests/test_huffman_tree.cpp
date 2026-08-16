#include "huffman_tree.h"
#include "test_utils.h"

// Проверяет, что ни один код в таблице не является префиксом другого —
// это ключевое свойство, обеспечивающее однозначную декодируемость.
bool isPrefixFree(const CodeTable& table) {
    for (const auto& [symbolA, codeA] : table) {
        for (const auto& [symbolB, codeB] : table) {
            if (symbolA == symbolB) {
                continue;
            }
            if (codeA.size() <= codeB.size() && codeB.compare(0, codeA.size(), codeA) == 0) {
                return false;
            }
        }
    }
    return true;
}

void test_classic_frequencies() {
    // Классический пример из учебников по алгоритму Хаффмана
    FrequencyTable freq{};
    freq['a'] = 5;
    freq['b'] = 9;
    freq['c'] = 12;
    freq['d'] = 13;
    freq['e'] = 16;
    freq['f'] = 45;

    HuffmanTree tree;
    tree.build(freq);
    CodeTable codes = tree.buildCodeTable();

    CHECK(codes.size() == 6);
    CHECK(isPrefixFree(codes));
    // самый частый символ должен получить код не длиннее, чем у редкого
    CHECK(codes['f'].size() <= codes['a'].size());
}

void test_single_symbol_gets_one_bit_code() {
    FrequencyTable freq{};
    freq['x'] = 100;

    HuffmanTree tree;
    tree.build(freq);
    CodeTable codes = tree.buildCodeTable();

    CHECK(codes.size() == 1);
    CHECK(codes['x'] == "0");
}

void test_two_symbols_get_single_bit_codes() {
    FrequencyTable freq{};
    freq['a'] = 1;
    freq['b'] = 1;

    HuffmanTree tree;
    tree.build(freq);
    CodeTable codes = tree.buildCodeTable();

    CHECK(codes.size() == 2);
    CHECK(isPrefixFree(codes));
    CHECK(codes['a'].size() == 1);
    CHECK(codes['b'].size() == 1);
}

void test_empty_frequency_table_gives_empty_tree() {
    FrequencyTable freq{};  // все частоты равны нулю

    HuffmanTree tree;
    tree.build(freq);
    CodeTable codes = tree.buildCodeTable();

    CHECK(codes.empty());
    CHECK(tree.getRoot() == nullptr);
}

void test_build_is_deterministic() {
    // Одна и та же таблица частот должна давать одинаковое дерево
    // (это важно: дерево перестраивается заново при разжатии).
    FrequencyTable freq{};
    freq['a'] = 3;
    freq['b'] = 3;
    freq['c'] = 7;

    HuffmanTree tree1, tree2;
    tree1.build(freq);
    tree2.build(freq);

    CodeTable codes1 = tree1.buildCodeTable();
    CodeTable codes2 = tree2.buildCodeTable();

    CHECK(codes1.size() == codes2.size());
    for (const auto& [symbol, code] : codes1) {
        CHECK(codes2.count(symbol) == 1);
        CHECK(codes2[symbol] == code);
    }
}

int main() {
    RUN_TEST(test_classic_frequencies);
    RUN_TEST(test_single_symbol_gets_one_bit_code);
    RUN_TEST(test_two_symbols_get_single_bit_codes);
    RUN_TEST(test_empty_frequency_table_gives_empty_tree);
    RUN_TEST(test_build_is_deterministic);
    return reportResults();
}
