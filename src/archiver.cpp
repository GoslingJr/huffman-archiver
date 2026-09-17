#include "archiver.h"
#include "bit_io.h"
#include "huffman_tree.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

void Archiver::compress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Не удалось открыть входной файл: " + inputPath);
    }

    std::vector<unsigned char> data((std::istreambuf_iterator<char>(in)),
                                    std::istreambuf_iterator<char>());
    in.close();

    std::array<uint64_t, 256> frequencies{};
    for (unsigned char byte : data) {
        frequencies[byte]++;
    }

    BitWriter writer(outputPath);
    if (!writer.isOpen()) {
        throw std::runtime_error("Не удалось создать выходной файл: " + outputPath);
    }

    uint64_t originalSize = data.size();
    writer.writeUint64(originalSize);

    if (originalSize == 0) {
        writer.writeUint16(0);
        return;
    }

    uint16_t uniqueCount = 0;
    for (int i = 0; i < 256; ++i) {
        if (frequencies[i] > 0) {
            uniqueCount++;
        }
    }
    writer.writeUint16(uniqueCount);

    for (int i = 0; i < 256; ++i) {
        if (frequencies[i] > 0) {
            writer.writeByte(static_cast<unsigned char>(i));
            writer.writeUint64(frequencies[i]);
        }
    }

    HuffmanTree tree;
    tree.build(frequencies);
    std::unordered_map<unsigned char, std::string> codes = tree.buildCodeTable();

    for (unsigned char byte : data) {
        const std::string& code = codes[byte];
        for (char bit : code) {
            writer.writeBit(bit == '1');
        }
    }

    writer.flush();
}

void Archiver::decompress(const std::string& inputPath, const std::string& outputPath) {
    BitReader reader(inputPath);
    if (!reader.isOpen()) {
        throw std::runtime_error("Не удалось открыть входной файл: " + inputPath);
    }

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) {
        throw std::runtime_error("Не удалось создать выходной файл: " + outputPath);
    }

    uint64_t originalSize = reader.readUint64();
    uint16_t uniqueCount = reader.readUint16();

    if (originalSize == 0 || uniqueCount == 0) {
        return;  // исходный файл был пустым
    }

    // Уникальных байтовых значений не может быть больше 256 — если заголовок
    // утверждает обратное, файл повреждён или это не .huf-файл вовсе.
    if (uniqueCount > 256) {
        throw std::runtime_error("Повреждённый файл: некорректный заголовок (" + inputPath + ")");
    }

    std::array<uint64_t, 256> frequencies{};
    for (uint16_t i = 0; i < uniqueCount; ++i) {
        unsigned char symbol = reader.readByte();
        uint64_t frequency = reader.readUint64();
        frequencies[symbol] = frequency;
    }

    HuffmanTree tree;
    tree.build(frequencies);

    for (uint64_t decoded = 0; decoded < originalSize; ++decoded) {
        out.put(static_cast<char>(tree.decodeSymbol(reader)));
    }
}
