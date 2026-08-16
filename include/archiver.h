#pragma once
#include <string>

// Формат сжатого файла (.huf):
//   [8 байт]  originalSize   — размер исходного файла в байтах
//   [2 байта] uniqueCount    — количество уникальных символов (0, если файл пуст)
//   uniqueCount раз повторяется:
//     [1 байт]  символ
//     [8 байт]  его частота в исходном файле
//   далее — сами данные, закодированные префиксными кодами Хаффмана,
//   упакованные по 8 бит в байт (последний байт может быть дополнен нулями).
class Archiver {
public:
    // Сжимает файл inputPath в outputPath. Возвращает false при ошибке.
    static bool compress(const std::string& inputPath, const std::string& outputPath);

    // Разжимает файл inputPath (созданный compress) в outputPath.
    static bool decompress(const std::string& inputPath, const std::string& outputPath);
};
