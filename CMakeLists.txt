cmake_minimum_required(VERSION 3.10)
project(HuffmanArchiver CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Санитайзеры для C/C++ (см. чеклист по оформлению репозитория, п.10):
# cmake -B build -DHUFFMAN_ENABLE_SANITIZERS=ON
# Собирает и линкует всё с AddressSanitizer + UndefinedBehaviorSanitizer.
option(HUFFMAN_ENABLE_SANITIZERS "Build with AddressSanitizer + UndefinedBehaviorSanitizer" OFF)

add_compile_options(-Wall -Wextra)

if(HUFFMAN_ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g -O1)
    add_link_options(-fsanitize=address,undefined)
endif()

include_directories(include)

# Библиотека с основной логикой (используется и в приложении, и в тестах)
add_library(huffman_core
    src/bit_io.cpp
    src/huffman_tree.cpp
    src/archiver.cpp
)

# Основное консольное приложение
add_executable(huffman src/main.cpp)
target_link_libraries(huffman PRIVATE huffman_core)

# Тесты
enable_testing()
include_directories(tests)

add_executable(test_bit_io tests/test_bit_io.cpp)
target_link_libraries(test_bit_io PRIVATE huffman_core)
add_test(NAME BitIOTests COMMAND test_bit_io)

add_executable(test_huffman_tree tests/test_huffman_tree.cpp)
target_link_libraries(test_huffman_tree PRIVATE huffman_core)
add_test(NAME HuffmanTreeTests COMMAND test_huffman_tree)

add_executable(test_archiver tests/test_archiver.cpp)
target_link_libraries(test_archiver PRIVATE huffman_core)
add_test(NAME ArchiverTests COMMAND test_archiver)
