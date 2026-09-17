#include "bit_io.h"

#include <stdexcept>

// ---------- BitWriter ----------

BitWriter::BitWriter(const std::string& filename)
    : out_(filename, std::ios::binary), buffer_(0), bitCount_(0) {}

BitWriter::~BitWriter() {
    flush();
}

void BitWriter::writeBit(bool bit) {
    buffer_ <<= 1;
    if (bit) {
        buffer_ |= 1;
    }
    bitCount_++;

    if (bitCount_ == 8) {
        out_.put(static_cast<char>(buffer_));
        buffer_ = 0;
        bitCount_ = 0;
    }
}

void BitWriter::writeByte(unsigned char byte) {
    out_.put(static_cast<char>(byte));
}

void BitWriter::writeUint16(uint16_t value) {
    writeByte(static_cast<unsigned char>((value >> 8) & 0xFF));
    writeByte(static_cast<unsigned char>(value & 0xFF));
}

void BitWriter::writeUint64(uint64_t value) {
    for (int i = 7; i >= 0; --i) {
        writeByte(static_cast<unsigned char>((value >> (i * 8)) & 0xFF));
    }
}

void BitWriter::flush() {
    if (bitCount_ > 0) {
        // дополняем незаполненный байт нулями справа
        buffer_ = static_cast<unsigned char>(buffer_ << (8 - bitCount_));
        out_.put(static_cast<char>(buffer_));
        buffer_ = 0;
        bitCount_ = 0;
    }
    out_.flush();
}

bool BitWriter::isOpen() const {
    return out_.is_open();
}

// ---------- BitReader ----------

BitReader::BitReader(const std::string& filename)
    : in_(filename, std::ios::binary), buffer_(0), bitPos_(0) {}

BitReader::~BitReader() = default;

bool BitReader::readBit(bool& bit) {
    if (bitPos_ == 0) {
        int c = in_.get();
        if (c == EOF) {
            return false;
        }
        buffer_ = static_cast<unsigned char>(c);
        bitPos_ = 8;
    }
    bitPos_--;
    bit = ((buffer_ >> bitPos_) & 1) != 0;
    return true;
}

unsigned char BitReader::readByte() {
    int c = in_.get();
    if (c == EOF) {
        throw std::runtime_error("Неожиданный конец файла при чтении заголовка");
    }
    return static_cast<unsigned char>(c);
}

uint16_t BitReader::readUint16() {
    uint16_t hi = readByte();
    uint16_t lo = readByte();
    return static_cast<uint16_t>((hi << 8) | lo);
}

uint64_t BitReader::readUint64() {
    uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value = (value << 8) | readByte();
    }
    return value;
}

bool BitReader::isOpen() const {
    return in_.is_open();
}
