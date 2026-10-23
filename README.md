# Slot Engine Study (C++17)

A small educational C++ project inspired by the kind of separation you may encounter in a game-math or simulation codebase. It is **not** a copy of 7Bridges Maths' internal software, and its toy payout rules are not a real or certified game model.

The project deliberately keeps the core small so you can study its architecture, tests, memory behavior, and throughput without distractions from a GUI, network APIs, or third-party dependencies.

## What it does

- Simulates a three-reel, four-symbol toy slot.
- Applies a simple payout rule when all three symbols match.
- Accepts a reproducible random seed.
- Runs a large number of spins and reports total wagered units, payout units, winning spins, simulated payout rate, elapsed time, and throughput.
- Includes basic tests and builds with CMake.

## Architecture

```text
include/slot_engine/
  domain.hpp        # Symbol, fixed reel array, configuration, result
  evaluator.hpp     # Public interface for deterministic evaluation
  game_engine.hpp   # Owns RNG and produces spin results
  simulator.hpp     # Aggregates metrics across many spins
src/
  evaluator.cpp     # Pure rule evaluation
  game_engine.cpp   # Random spin generation
  simulator.cpp     # Simulation loop and counters
  main.cpp          # CLI / output
 tests/
  test_engine.cpp   # Focused tests, no external test framework
```

The split is intentionally modest:

- **Domain:** the concepts and data types of this toy game.
- **Evaluation:** pure function, deterministic and easy to unit test.
- **Engine:** randomness and creation of a spin.
- **Simulation:** repeated execution and aggregation of counters.
- **CLI:** input/output only, not mixed into the domain logic.

This is a simple layered design, not full MVC: the project has no GUI or controller. Keeping UI concerns out of the core would let a GUI or design tool call this code later through an appropriate interface.

## Why these data structures?

- `enum class Symbol`: a bounded set of symbol values, with better type safety than unexplained integers.
- `std::array<Symbol, 3>`: the number of reels is fixed in this example. It stores the three symbols inline and avoids a heap allocation for each spin's reel collection.
- `std::array<uint32_t, kSymbolCount>`: payout lookup has a fixed number of symbol types. Direct indexing is simple and avoids hashing or tree traversal.
- `struct SpinResult` and `struct SimulationReport`: related values are grouped into meaningful types.
- `std::mt19937_64`: a seedable standard pseudo-random engine, so tests and runs can be reproduced.
- `uint64_t` counters: accommodates larger simulation totals than a small signed integer.

This example deliberately does not use linked lists, trees, or `unordered_map` just because they are available. Production code should choose a structure based on access patterns and measurement. For this fixed-size problem, arrays are simpler and appropriate. If the number of reels or symbols were loaded dynamically, `std::vector` might make more sense.

## Build and run (Linux / WSL / macOS with CMake and a C++ compiler)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/slot_simulator 1000000 42
```

Arguments are optional: `slot_simulator [spin_count] [seed]`. Defaults are 1,000,000 spins and seed 42.

The `Payout rate` is a toy simulation metric calculated as total payout divided by total wagered units. It is only a code-learning demonstration, not a prediction about any real game.

## Build with Visual Studio on Windows

The project uses standard C++17 and CMake, so you can open the folder directly in a recent Visual Studio version with the **Desktop development with C++** workload installed. Choose **File → Open → Folder** and select `slot_engine_study`; Visual Studio detects `CMakeLists.txt`. Select `slot_simulator` as the startup target to run the sample.

Alternatively, in a Visual Studio Developer PowerShell opened at the project folder:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\slot_simulator.exe 1000000 42
```

Use Visual Studio's **Performance Profiler** to inspect CPU cost and its memory diagnostics / AddressSanitizer support where available. **Valgrind is intended for Linux environments**, not ordinary native Windows builds; on Windows, run the project under WSL2 with a Linux distribution if you specifically want to practice Valgrind.

## Memory and correctness checks

### 1. Valgrind Memcheck (Linux / WSL with Valgrind installed)

Build a debug version first:

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug -j
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./build-debug/slot_simulator 100000 42
```

Look for invalid reads/writes, use of uninitialized values, and definitely/indirectly lost memory. This project uses fixed arrays and standard-library objects, so it intentionally has no manual `new`/`delete` in the spin loop. A clean run is useful evidence, but does not prove that every possible bug is absent.

### 2. AddressSanitizer and UndefinedBehaviorSanitizer

GCC or Clang:

```bash
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DSLOT_ENABLE_SANITIZERS=ON
cmake --build build-asan -j
ctest --test-dir build-asan --output-on-failure
./build-asan/slot_simulator 100000 42
```

These sanitizers help identify invalid memory access and various forms of undefined behavior. Availability depends on compiler/platform.

### 3. Callgrind (optional)

```bash
valgrind --tool=callgrind ./build/slot_simulator 1000000 42
```

Open the generated `callgrind.out.*` with a compatible viewer such as KCachegrind/QCachegrind to inspect where execution cost accumulates. Valgrind changes runtime performance, so do **not** use its elapsed time as the application's true throughput benchmark.

## Performance exercise

Run the exact same release build several times, using the same seed and spin count. Record the reported throughput; use a median rather than the single fastest run. Close heavy background tasks and compare like with like.

Possible experiments:

1. Increase the simulation to 10 million spins and observe scaling.
2. Add a second evaluation rule, write tests, and measure the change.
3. Compare an array-based fixed lookup with a map-based lookup in a separate benchmark, keeping behavior identical.
4. Profile before optimizing. Do not assume a data structure is faster without measuring the real access pattern.
5. Keep correctness tests passing before and after performance changes.

## Study path

1. Read `domain.hpp`: types and representation.
2. Read `evaluator.cpp`: deterministic business logic.
3. Read `game_engine.cpp`: pseudo-random input and orchestration.
4. Read `simulator.cpp`: hot loop and aggregation.
5. Read `test_engine.cpp`: how expected behavior is checked.
6. Run the app and tests.
7. Run Valgrind / sanitizers.
8. Change one behavior, add a test, then profile.

## Important limitation

`std::mt19937_64` is used only to learn reproducible simulations. A real commercial game can require a mathematically validated model, specified randomness requirements, extensive statistical testing, security review, and jurisdictional certification. This sample is not designed or certified for real-money use.
