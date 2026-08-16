#pragma once
#include <cstdint>
#include <fstream>
#include <string>

// BitWriter пишет отдельные биты в файл, накапливая их в однобайтовом буфере.
// Когда в буфере накопится 8 бит, они сбрасываются в файл одним байтом.
class BitWriter {
public:
    explicit BitWriter(const std::string& filename);
    ~BitWriter();

    void writeBit(bool bit);
    void writeByte(unsigned char byte);
    void writeUint16(uint16_t value);
    void writeUint64(uint64_t value);

    // Досрочно сбрасывает буфер, дополняя его нулями, если он не полон.
    void flush();

    bool isOpen() const;

private:
    std::ofstream out_;
    unsigned char buffer_;
    int bitCount_;
};

// BitReader читает биты из файла, записанного BitWriter'ом.
class BitReader {
public:
    explicit BitReader(const std::string& filename);
    ~BitReader();

    // Возвращает false, если файл закончился.
    bool readBit(bool& bit);
    unsigned char readByte();
    uint16_t readUint16();
    uint64_t readUint64();

    bool isOpen() const;

private:
    std::ifstream in_;
    unsigned char buffer_;
    int bitPos_; // сколько бит буфера ещё не прочитано
};
