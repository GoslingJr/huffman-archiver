#!/usr/bin/env python3
"""
Генерирует тестовые файлы разных типов и размеров для экспериментов.

Типы данных:
  - text        : связный английский текст (Zipf-распределение символов) -
                   хорошо сжимается, т.к. распределение символов неравномерное
  - random      : случайные байты (равномерное распределение) -
                   почти не сжимается, теоретический "плохой" случай для Хаффмана
  - repetitive  : сильно повторяющиеся данные (мало уникальных символов) -
                   сжимается очень хорошо
  - already_compressed : имитация уже сжатых данных (случайные байты) -
                   демонстрирует, что повторное сжатие бессмысленно
"""
import os
import random
import string

OUTPUT_DIR = os.path.join(os.path.dirname(__file__), "data")
SIZES = [1_000, 10_000, 100_000, 1_000_000, 5_000_000]  # байт

WORDS = (
    "the quick brown fox jumps over lazy dog while sun rises above quiet "
    "hills and river flows gently through forest where birds sing every "
    "morning bringing joy to travelers who walk along old stone path"
).split()


def make_text(size_bytes: int, seed: int) -> bytes:
    rng = random.Random(seed)
    parts = []
    total = 0
    while total < size_bytes:
        word = rng.choice(WORDS)
        parts.append(word)
        total += len(word) + 1
    text = " ".join(parts)
    return text.encode("utf-8")[:size_bytes]


def make_random(size_bytes: int, seed: int) -> bytes:
    rng = random.Random(seed)
    return bytes(rng.randrange(256) for _ in range(size_bytes))


def make_repetitive(size_bytes: int, seed: int) -> bytes:
    rng = random.Random(seed)
    alphabet = rng.sample(string.ascii_lowercase, 3)
    return bytes(ord(rng.choice(alphabet)) for _ in range(size_bytes))


def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    generators = {
        "text": make_text,
        "random": make_random,
        "repetitive": make_repetitive,
    }

    manifest = []
    for kind, generator in generators.items():
        for size in SIZES:
            data = generator(size, seed=size)
            filename = f"{kind}_{size}.bin"
            path = os.path.join(OUTPUT_DIR, filename)
            with open(path, "wb") as f:
                f.write(data)
            manifest.append((kind, size, path))
            print(f"создан {path} ({len(data)} байт)")

    return manifest


if __name__ == "__main__":
    main()
