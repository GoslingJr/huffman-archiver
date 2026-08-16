#include "archiver.h"
#include "test_utils.h"

#include <cstdio>
#include <fstream>
#include <vector>

namespace {

std::vector<unsigned char> readFile(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    return std::vector<unsigned char>((std::istreambuf_iterator<char>(in)),
                                       std::istreambuf_iterator<char>());
}

void writeFile(const std::string& path, const std::vector<unsigned char>& data) {
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
}

// Проверяет полный цикл: записать файл -> сжать -> разжать -> сравнить с оригиналом.
void checkRoundtrip(const std::string& name, const std::vector<unsigned char>& data) {
    std::string original = name + "_orig.tmp";
    std::string compressed = name + "_comp.tmp";
    std::string restored = name + "_rest.tmp";

    writeFile(original, data);

    CHECK(Archiver::compress(original, compressed));
    CHECK(Archiver::decompress(compressed, restored));

    std::vector<unsigned char> result = readFile(restored);
    CHECK(result == data);

    std::remove(original.c_str());
    std::remove(compressed.c_str());
    std::remove(restored.c_str());
}

}  // namespace

void test_roundtrip_text() {
    std::vector<unsigned char> data(
        {'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!',
         ' ', 'h', 'e', 'l', 'l', 'o', ' ', 'a', 'g', 'a', 'i', 'n'});
    checkRoundtrip("text", data);
}

void test_roundtrip_empty_file() {
    checkRoundtrip("empty", {});
}

void test_roundtrip_single_byte() {
    checkRoundtrip("single", {'z'});
}

void test_roundtrip_repeated_symbol() {
    // Крайний случай: только один уникальный символ во всём файле —
    // дерево вырождается в единственный узел без потомков.
    std::vector<unsigned char> data(1000, 'a');
    checkRoundtrip("repeated", data);
}

void test_roundtrip_binary_data() {
    std::vector<unsigned char> data;
    for (int i = 0; i < 2000; ++i) {
        data.push_back(static_cast<unsigned char>((i * 37) % 256));
    }
    checkRoundtrip("binary", data);
}

void test_compressed_file_is_smaller_for_text() {
    // На тексте с неравномерным распределением символов сжатый файл
    // должен получиться меньше исходного.
    std::string text;
    for (int i = 0; i < 500; ++i) {
        text += "the quick brown fox jumps over the lazy dog ";
    }
    std::vector<unsigned char> data(text.begin(), text.end());

    std::string original = "ratio_orig.tmp";
    std::string compressed = "ratio_comp.tmp";
    writeFile(original, data);
    CHECK(Archiver::compress(original, compressed));

    std::ifstream in(compressed, std::ios::binary | std::ios::ate);
    auto compressedSize = in.tellg();

    CHECK(static_cast<size_t>(compressedSize) < data.size());

    std::remove(original.c_str());
    std::remove(compressed.c_str());
}

int main() {
    RUN_TEST(test_roundtrip_text);
    RUN_TEST(test_roundtrip_empty_file);
    RUN_TEST(test_roundtrip_single_byte);
    RUN_TEST(test_roundtrip_repeated_symbol);
    RUN_TEST(test_roundtrip_binary_data);
    RUN_TEST(test_compressed_file_is_smaller_for_text);
    return reportResults();
}
