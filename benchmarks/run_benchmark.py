#!/usr/bin/env python3
"""
Замеряет степень сжатия и скорость работы huffman на файлах разных типов и
размеров. Каждый замер повторяется REPEATS раз, считается среднее и
среднеквадратичное отклонение. Результаты сохраняются в results.csv и
в виде графиков (PNG) в текущей директории.

Использование:
    python3 generate_data.py     # сначала сгенерировать тестовые файлы
    python3 run_benchmark.py     # затем прогнать замеры
"""
import csv
import os
import statistics
import subprocess
import time

BENCH_DIR = os.path.dirname(__file__)
DATA_DIR = os.path.join(BENCH_DIR, "data")
BUILD_DIR = os.path.join(os.path.dirname(BENCH_DIR), "build")
HUFFMAN_BIN_NAME = "huffman.exe" if os.name == "nt" else "huffman"
HUFFMAN_BIN = os.path.join(BUILD_DIR, HUFFMAN_BIN_NAME)
TMP_DIR = os.path.join(BENCH_DIR, "tmp")
REPEATS = 5


def run_huffman(mode, input_path, output_path):
    start = time.perf_counter()
    result = subprocess.run(
        [HUFFMAN_BIN, mode, input_path, output_path],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
    )
    elapsed = time.perf_counter() - start
    if result.returncode != 0:
        raise RuntimeError(f"huffman завершился с ошибкой: {result.stderr.decode()}")
    return elapsed


def benchmark_file(path):
    os.makedirs(TMP_DIR, exist_ok=True)
    compressed_path = os.path.join(TMP_DIR, "bench.huf")
    restored_path = os.path.join(TMP_DIR, "bench_restored")

    compress_times = []
    decompress_times = []

    for _ in range(REPEATS):
        compress_times.append(run_huffman("-c", path, compressed_path))
        decompress_times.append(run_huffman("-d", compressed_path, restored_path))

    original_size = os.path.getsize(path)
    compressed_size = os.path.getsize(compressed_path)

    with open(path, "rb") as f1, open(restored_path, "rb") as f2:
        assert f1.read() == f2.read(), f"разжатый файл не совпадает с оригиналом: {path}"

    return {
        "original_size": original_size,
        "compressed_size": compressed_size,
        "compression_ratio": original_size / compressed_size if compressed_size else float("inf"),
        "compress_time_mean": statistics.mean(compress_times),
        "compress_time_stdev": statistics.stdev(compress_times) if len(compress_times) > 1 else 0.0,
        "decompress_time_mean": statistics.mean(decompress_times),
        "decompress_time_stdev": statistics.stdev(decompress_times) if len(decompress_times) > 1 else 0.0,
    }


def parse_filename(filename):
    # формат: <тип>_<размер>.bin
    name = filename.rsplit(".", 1)[0]
    kind, size = name.rsplit("_", 1)
    return kind, int(size)


def main():
    if not os.path.exists(HUFFMAN_BIN):
        raise SystemExit(f"Не найден исполняемый файл {HUFFMAN_BIN}. Сначала соберите проект (make).")

    files = sorted(f for f in os.listdir(DATA_DIR) if f.endswith(".bin"))
    if not files:
        raise SystemExit(f"В {DATA_DIR} нет тестовых файлов. Сначала запустите generate_data.py")

    rows = []
    for filename in files:
        kind, size = parse_filename(filename)
        path = os.path.join(DATA_DIR, filename)
        print(f"Замер: {filename} ...", end=" ", flush=True)
        stats = benchmark_file(path)
        rows.append({"type": kind, "size": size, **stats})
        print(
            f"ratio={stats['compression_ratio']:.2f}x  "
            f"compress={stats['compress_time_mean']*1000:.2f}±{stats['compress_time_stdev']*1000:.2f} ms  "
            f"decompress={stats['decompress_time_mean']*1000:.2f}±{stats['decompress_time_stdev']*1000:.2f} ms"
        )

    csv_path = os.path.join(BENCH_DIR, "results.csv")
    with open(csv_path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)
    print(f"\nРезультаты сохранены в {csv_path}")

    make_plots(rows)


def make_plots(rows):
    import matplotlib.pyplot as plt

    types = sorted(set(r["type"] for r in rows))
    colors = {"text": "tab:blue", "random": "tab:red", "repetitive": "tab:green"}

    # --- График 1: степень сжатия от размера файла ---
    plt.figure(figsize=(8, 5))
    for kind in types:
        pts = sorted((r["size"], r["compression_ratio"]) for r in rows if r["type"] == kind)
        xs, ys = zip(*pts)
        plt.plot(xs, ys, marker="o", label=kind, color=colors.get(kind))
    plt.xscale("log")
    plt.xlabel("Размер исходного файла, байт")
    plt.ylabel("Степень сжатия (original/compressed)")
    plt.title("Степень сжатия в зависимости от размера и типа файла")
    plt.legend()
    plt.grid(True, which="both", alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(BENCH_DIR, "compression_ratio.png"), dpi=120)
    plt.close()

    # --- График 2: скорость сжатия от размера файла ---
    plt.figure(figsize=(8, 5))
    for kind in types:
        pts = sorted(
            (r["size"], r["compress_time_mean"], r["compress_time_stdev"])
            for r in rows if r["type"] == kind
        )
        xs, ys, errs = zip(*pts)
        plt.errorbar(xs, ys, yerr=errs, marker="o", label=kind, color=colors.get(kind), capsize=3)
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Размер исходного файла, байт")
    plt.ylabel("Время сжатия, сек")
    plt.title("Скорость сжатия в зависимости от размера и типа файла")
    plt.legend()
    plt.grid(True, which="both", alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(BENCH_DIR, "compress_speed.png"), dpi=120)
    plt.close()

    # --- График 3: скорость разжатия от размера файла ---
    plt.figure(figsize=(8, 5))
    for kind in types:
        pts = sorted(
            (r["size"], r["decompress_time_mean"], r["decompress_time_stdev"])
            for r in rows if r["type"] == kind
        )
        xs, ys, errs = zip(*pts)
        plt.errorbar(xs, ys, yerr=errs, marker="o", label=kind, color=colors.get(kind), capsize=3)
    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("Размер исходного файла, байт")
    plt.ylabel("Время разжатия, сек")
    plt.title("Скорость разжатия в зависимости от размера и типа файла")
    plt.legend()
    plt.grid(True, which="both", alpha=0.3)
    plt.tight_layout()
    plt.savefig(os.path.join(BENCH_DIR, "decompress_speed.png"), dpi=120)
    plt.close()

    print("Графики сохранены: compression_ratio.png, compress_speed.png, decompress_speed.png")


if __name__ == "__main__":
    main()
