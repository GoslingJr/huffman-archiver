#include "archiver.h"
#include "bit_io.h"
#include "huffman_tree.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

bool Archiver::compress(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream in(inputPath, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Не удалось открыть входной файл: " << inputPath << std::endl;
        return false;
    }

    std::vector<unsigned char> data((std::istreambuf_iterator<char>(in)),
                                     std::istreambuf_iterator<char>());
    in.close();

    FrequencyTable frequencies{};
    for (unsigned char byte : data) {
        frequencies[byte]++;
    }

    BitWriter writer(outputPath);
    if (!writer.isOpen()) {
        std::cerr << "Не удалось создать выходной файл: " << outputPath << std::endl;
        return false;
    }

    uint64_t originalSize = data.size();
    writer.writeUint64(originalSize);

    if (originalSize == 0) {
        writer.writeUint16(0);
        return true;
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
    CodeTable codes = tree.buildCodeTable();

    for (unsigned char byte : data) {
        const std::string& code = codes[byte];
        for (char bit : code) {
            writer.writeBit(bit == '1');
        }
    }

    writer.flush();
    return true;
}

bool Archiver::decompress(const std::string& inputPath, const std::string& outputPath) {
    BitReader reader(inputPath);
    if (!reader.isOpen()) {
        std::cerr << "Не удалось открыть входной файл: " << inputPath << std::endl;
        return false;
    }

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Не удалось создать выходной файл: " << outputPath << std::endl;
        return false;
    }

    uint64_t originalSize = reader.readUint64();
    uint16_t uniqueCount = reader.readUint16();

    if (originalSize == 0 || uniqueCount == 0) {
        return true;  // исходный файл был пустым
    }

    FrequencyTable frequencies{};
    for (uint16_t i = 0; i < uniqueCount; ++i) {
        unsigned char symbol = reader.readByte();
        uint64_t frequency = reader.readUint64();
        frequencies[symbol] = frequency;
    }

    HuffmanTree tree;
    tree.build(frequencies);
    HuffmanNodePtr root = tree.getRoot();

    uint64_t decoded = 0;

    if (root->isLeaf()) {
        // Особый случай: единственный уникальный символ во всём файле.
        while (decoded < originalSize) {
            bool bit;
            if (!reader.readBit(bit)) {
                break;
            }
            out.put(static_cast<char>(root->symbol));
            decoded++;
        }
        return true;
    }

    HuffmanNodePtr current = root;
    bool bit;
    while (decoded < originalSize && reader.readBit(bit)) {
        current = bit ? current->right : current->left;
        if (current->isLeaf()) {
            out.put(static_cast<char>(current->symbol));
            decoded++;
            current = root;
        }
    }

    return true;
}
