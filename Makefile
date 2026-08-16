CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Iinclude
BUILD = build

CORE_SRC = src/bit_io.cpp src/huffman_tree.cpp src/archiver.cpp
CORE_OBJ = $(CORE_SRC:src/%.cpp=$(BUILD)/%.o)

# На Windows make (mingw32-make) сам определяет переменную OS=Windows_NT,
# поэтому команды очистки/создания папок и расширение .exe выбираются
# автоматически под нужную платформу.
ifeq ($(OS),Windows_NT)
    EXE_EXT = .exe
    MKDIR_CMD = if not exist $(BUILD) mkdir $(BUILD)
    RM_CMD = if exist $(BUILD) rmdir /s /q $(BUILD)
else
    EXE_EXT =
    MKDIR_CMD = mkdir -p $(BUILD)
    RM_CMD = rm -rf $(BUILD)
endif

# Используем '>' вместо табуляции для строк рецептов — так Makefile не
# ломается, если табы случайно превратились в пробелы при копировании
# (частая проблема при вставке в VS Code / из чата).
.RECIPEPREFIX = >

.PHONY: all clean test

all: $(BUILD)/huffman$(EXE_EXT)

$(BUILD):
>$(MKDIR_CMD)

$(BUILD)/%.o: src/%.cpp | $(BUILD)
>$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/huffman$(EXE_EXT): $(CORE_OBJ) $(BUILD)/main.o
>$(CXX) $^ -o $@

test: $(CORE_OBJ)
>$(CXX) $(CXXFLAGS) -Itests tests/test_bit_io.cpp $(CORE_OBJ) -o $(BUILD)/test_bit_io$(EXE_EXT)
>$(CXX) $(CXXFLAGS) -Itests tests/test_huffman_tree.cpp $(CORE_OBJ) -o $(BUILD)/test_huffman_tree$(EXE_EXT)
>$(CXX) $(CXXFLAGS) -Itests tests/test_archiver.cpp $(CORE_OBJ) -o $(BUILD)/test_archiver$(EXE_EXT)
>$(BUILD)/test_bit_io$(EXE_EXT)
>$(BUILD)/test_huffman_tree$(EXE_EXT)
>$(BUILD)/test_archiver$(EXE_EXT)

clean:
>$(RM_CMD)
