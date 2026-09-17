#pragma once
#include <cstdint>
#include <fstream>
#include <string>

/**
 * @brief Пишет отдельные биты в файл, накапливая их в однобайтовом буфере.
 *
 * Когда в буфере накопится 8 бит, они сбрасываются в файл одним байтом.
 */
class BitWriter {
public:
    /**
     * @brief Открывает файл filename для бинарной записи.
     * @param filename путь к создаваемому файлу.
     */
    explicit BitWriter(const std::string& filename);

    /// Досрочно сбрасывает недописанный буфер перед закрытием файла.
    ~BitWriter();

    /// @brief Записывает один бит.
    void writeBit(bool bit);

    /// @brief Записывает один байт напрямую, минуя битовый буфер.
    void writeByte(unsigned char byte);

    /// @brief Записывает 16-битное число в порядке от старшего байта к младшему.
    void writeUint16(uint16_t value);

    /// @brief Записывает 64-битное число в порядке от старшего байта к младшему.
    void writeUint64(uint64_t value);

    /// @brief Досрочно сбрасывает буфер, дополняя его нулями, если он не полон.
    void flush();

    /// @return true, если файл открыт и доступен для записи.
    bool isOpen() const;

private:
    std::ofstream out_;
    unsigned char buffer_;
    int bitCount_;
};

/**
 * @brief Читает биты из файла, записанного BitWriter'ом.
 */
class BitReader {
public:
    /**
     * @brief Открывает файл filename для бинарного чтения.
     * @param filename путь к читаемому файлу.
     */
    explicit BitReader(const std::string& filename);

    ~BitReader();

    /**
     * @brief Читает один бит.
     * @param bit куда записать прочитанный бит.
     * @return false, если файл закончился.
     */
    bool readBit(bool& bit);

    /// @brief Читает один байт напрямую, минуя битовый буфер.
    unsigned char readByte();

    /// @brief Читает 16-битное число в порядке от старшего байта к младшему.
    uint16_t readUint16();

    /// @brief Читает 64-битное число в порядке от старшего байта к младшему.
    uint64_t readUint64();

    /// @return true, если файл открыт и доступен для чтения.
    bool isOpen() const;

private:
    std::ifstream in_;
    unsigned char buffer_;
    int bitPos_;  // сколько бит буфера ещё не прочитано
};
