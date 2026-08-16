#pragma once
#include <iostream>
#include <string>

// Простой самодельный тестовый фреймворк без внешних зависимостей:
// CHECK(condition) проверяет условие и печатает сообщение при провале,
// RUN_TEST(fn) запускает тестовую функцию с заголовком.

inline int g_testsRun = 0;
inline int g_testsFailed = 0;

#define CHECK(cond)                                                                   \
    do {                                                                              \
        g_testsRun++;                                                                 \
        if (!(cond)) {                                                                \
            g_testsFailed++;                                                          \
            std::cerr << "  [ОШИБКА] " << __FILE__ << ":" << __LINE__ << "  " << #cond \
                      << std::endl;                                                   \
        }                                                                             \
    } while (0)

#define RUN_TEST(fn)                                            \
    do {                                                        \
        std::cout << "Тест: " << #fn << std::endl;              \
        fn();                                                   \
    } while (0)

inline int reportResults() {
    std::cout << std::endl
              << (g_testsRun - g_testsFailed) << " / " << g_testsRun << " проверок пройдено"
              << std::endl;
    return g_testsFailed > 0 ? 1 : 0;
}
