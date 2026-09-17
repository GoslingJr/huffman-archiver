#pragma once
#include <string>

/**
 * @brief Архиватор файлов на основе алгоритма Хаффмана.
 *
 * Формат сжатого файла (.huf):
 *   [8 байт]  originalSize   — размер исходного файла в байтах
 *   [2 байта] uniqueCount    — количество уникальных символов (0, если файл пуст)
 *   uniqueCount раз повторяется:
 *     [1 байт]  символ
 *     [8 байт]  его частота в исходном файле
 *   далее — сами данные, закодированные префиксными кодами Хаффмана,
 *   упакованные по 8 бит в байт (последний байт может быть дополнен нулями).
 */
class Archiver {
public:
    /**
     * @brief Сжимает файл inputPath в outputPath.
     * @param inputPath путь к исходному файлу.
     * @param outputPath путь к создаваемому сжатому файлу.
     * @throws std::runtime_error если входной файл не удалось открыть или
     *         выходной файл не удалось создать.
     */
    static void compress(const std::string& inputPath, const std::string& outputPath);

    /**
     * @brief Разжимает файл inputPath (созданный compress) в outputPath.
     * @param inputPath путь к сжатому файлу.
     * @param outputPath путь к создаваемому разжатому файлу.
     * @throws std::runtime_error если входной файл не удалось открыть,
     *         выходной файл не удалось создать, либо сжатые данные
     *         повреждены или обрываются раньше, чем указывает заголовок.
     */
    static void decompress(const std::string& inputPath, const std::string& outputPath);
};
