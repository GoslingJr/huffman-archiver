#include <exception>
#include <iostream>
#include <string>

#include "archiver.h"

namespace {

void printUsage(const char* progName) {
    std::cout << "Архиватор на основе алгоритма Хаффмана\n\n"
              << "Использование:\n"
              << "  " << progName << " -c <входной_файл> <выходной_файл>   сжать файл\n"
              << "  " << progName << " -d <входной_файл> <выходной_файл>   разжать файл\n";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    std::string mode = argv[1];
    std::string input = argv[2];
    std::string output = argv[3];

    // Весь консольный ввод-вывод — здесь, в main; библиотечный код
    // (Archiver и всё под ним) ничего не печатает и сообщает об ошибках
    // через исключения.
    try {
        if (mode == "-c") {
            Archiver::compress(input, output);
            std::cout << "Файл успешно сжат: " << output << std::endl;
        } else if (mode == "-d") {
            Archiver::decompress(input, output);
            std::cout << "Файл успешно разжат: " << output << std::endl;
        } else {
            printUsage(argv[0]);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
