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

    bool ok = false;

    if (mode == "-c") {
        ok = Archiver::compress(input, output);
        if (ok) {
            std::cout << "Файл успешно сжат: " << output << std::endl;
        }
    } else if (mode == "-d") {
        ok = Archiver::decompress(input, output);
        if (ok) {
            std::cout << "Файл успешно разжат: " << output << std::endl;
        }
    } else {
        printUsage(argv[0]);
        return 1;
    }

    return ok ? 0 : 1;
}
