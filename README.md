# Assignment — Logarithmic Merging in C++

Implementation of a logarithmic merging strategy for dynamic inverted indexing, based on Information Retrieval (IIR Fig. 4.7).

## Assignment Overview

The main implementation is in:

| File | Description |
|---|---|
| `src/logarithmic.cpp` | `LogarithmicIndex` — logarithmic merging |

The project uses the provided on-disk format, in-memory auxiliary index, k-way merge implementation, drivers, tests, and experiment scripts.

## Build

Requirements: C++17 compiler and CMake 3.14+.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

On Windows, run executables from `build/Release/`.

## Run

```bash
./build/test_main data/tiny.txt work_tiny

./build/build_dynamic log data/tiny.txt work_t 14 out/tiny

./build/lookup out/tiny
```

The lookup program allows searching for terms and displaying their postings.

## Implementation

Implemented the `LogarithmicIndex` class in `src/logarithmic.cpp`.

The implementation includes:

- Generation-based disk index management.
- Logarithmic merging using a binary-counter strategy.
- Lookup across the in-memory index and non-empty disk generations.
- Finalization into a single output index.
- Cleanup of temporary indexes and cached readers.

## Performance Results

Experiments compared logarithmic merging with the auxiliary-index baseline.

At 100,000 indexed postings:

| Metric | Auxiliary | Logarithmic |
|---|---:|---:|
| Postings written | 165,012,346 | 30,383,731 |
| Write amplification | 29.04x | 5.35x |
| Maximum disk indexes | 1 | 5 |
| Runtime (seconds) | 11.68 | 9.14 |

The logarithmic approach reduced cumulative postings written by approximately **81.6%** at this measurement point.

## Technologies

- C++17
- CMake
- Information Retrieval
- Inverted Indexing
- Logarithmic Merging
- Performance Evaluation

## Repository Structure

```text
include/
src/
tools/
tests/
data/
results/
CMakeLists.txt
README.md
```

---

This project was completed as part of an Information Retrieval programming assignment.