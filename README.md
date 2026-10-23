# Slot Engine Study

An educational C++17 slot-machine project focused on deterministic game rules, reproducible simulation, and a small optional graphical client built with Raylib.

> **Educational project only.** This is a toy simulation, not a real-money gambling product or a certified game-math implementation.

## Features

- Three reels and four symbols: Cherry, Lemon, Bell, and Seven.
- Deterministic payout evaluation: three matching symbols trigger the configured payout.
- Seedable pseudo-random generation for repeatable runs.
- Command-line simulation with spin count, seed, payout totals, payout rate, elapsed time, and throughput.
- `GameSession` application layer for bet validation, balance accounting, and spin results.
- Optional Raylib desktop game with animated reel stops, selectable bets, credit and last-win panels, and mouse controls.
- CMake build, compiler warnings, and a focused test executable.

## Architecture

This project uses a lightweight **layered modular monolith**. It separates responsibilities without introducing unnecessary frameworks or services.

```text
CLI simulator ───────────────┐
                             ├──> slot_engine
slot_game (Raylib)           │      domain + evaluator + random engine
        │                    │
        └──> slot_application┘
              GameSession
              balance + bet validation + spin lifecycle
```

### Main components

| Component | Responsibility |
|---|---|
| `slot_engine` | Core types, symbol generation, payout evaluation, and simulation support. |
| `slot_application` | `GameSession`: validates bets, tracks the player's balance, delegates the spin to the engine, and reports the outcome. |
| `slot_simulator` | CLI program for running large batches of spins without initializing a graphical window. |
| `slot_game` | Optional Raylib desktop interface. Displays the authoritative result from `GameSession`; animation does not calculate or change payouts. |
| `slot_engine_tests` | Lightweight test executable covering payout rules, seed reproducibility, simulation counters, and session validation/accounting. |

The core and simulator do not require Raylib. Raylib is found and linked only when `SLOT_BUILD_GAME=ON`.

## Repository layout

```text
include/slot_engine/
  domain.hpp          # Symbols, reels, game configuration and spin result
  evaluator.hpp       # Deterministic payout evaluation interface
  game_engine.hpp     # Seedable random game engine
  game_session.hpp    # Application-level bet and balance management
  simulator.hpp       # Simulation metrics interface

src/
  evaluator.cpp       # Payout rules
  game_engine.cpp     # Reel generation and spin orchestration
  game_session.cpp    # Session validation and balance updates
  simulator.cpp       # Batch simulation and aggregation
  main.cpp            # CLI simulator entry point
  slot_game.cpp       # Optional Raylib graphical application

tests/
  test_engine.cpp     # Core and GameSession tests

assets/
  README.md           # Resource guidelines and optional asset locations
  symbols/            # Reserved for optional symbol artwork
  backgrounds/        # Reserved for optional backgrounds
  ui/                 # Optional slot-display.ttf font
  sounds/             # Reserved for optional sound effects
```

## Graphical game

The optional application is titled **Lucky Three** and uses a navy, gold, and ivory arcade-inspired palette. The four symbols are drawn with Raylib geometry, so no image pack is required.

Current interface features:
- Three animated reels with staggered stop times.
- Selectable bets of 1, 2, 5, or 10 credits.
- Initial balance of 100 educational credits.
- Credits, current bet, and last-win display.
- SPIN and bet controls disabled during the reel animation.
- Resizable window with a minimum size.
- Raylib's default font as a fallback when `assets/ui/slot-display.ttf` is absent or unreadable.

Sound effects and bundled image assets have not been added. Resource directories may be empty. Third-party assets should only be added with compatible licenses, with their source and license documented.

## Requirements

### CLI and tests

- CMake
- A C++17 compiler (MSVC, GCC, or Clang)
- CTest, normally installed with CMake

### Graphical application

- The requirements above
- Raylib with its CMake package available to `find_package(raylib CONFIG REQUIRED)`
- On Windows with vcpkg, install the `x64-windows` Raylib package and pass the vcpkg toolchain to CMake.

## Build and run on Windows (Visual Studio + vcpkg)

The commands below assume:
- Visual Studio 2022 with the **Desktop development with C++** workload.
- CMake installed at `C:\Program Files\CMake\bin\cmake.exe`.
- vcpkg installed at `%USERPROFILE%\source\vcpkg`.
- Raylib installed for the `x64-windows` triplet.

Open PowerShell in the repository root:

```powershell
$cmake = "C:\Program Files\CMake\bin\cmake.exe"
$ctest = "C:\Program Files\CMake\bin\ctest.exe"
$vcpkgRoot = "$env:USERPROFILE\source\vcpkg"
$toolchain = "$vcpkgRoot\scripts\buildsystems\vcpkg.cmake"

# Install Raylib once if it is not already installed.
& "$vcpkgRoot\vcpkg.exe" install raylib:x64-windows

# Configure a graphical build in a dedicated directory.
& $cmake -S . -B build-raylib -G "Visual Studio 17 2022" -A x64 `
  "-DCMAKE_TOOLCHAIN_FILE=$toolchain" `
  -DVCPKG_TARGET_TRIPLET=x64-windows `
  -DSLOT_BUILD_GAME=ON

# Build all targets: simulator, application, tests and graphical game.
& $cmake --build build-raylib --config Release

# Run automated tests.
& $ctest --test-dir build-raylib -C Release --output-on-failure

# Run 100,000 deterministic-seed simulation spins.
.\build-raylib\Release\slot_simulator.exe 100000 42

# Launch the GUI.
.\build-raylib\Release\slot_game.exe
```

If you want to build only the simulator and tests without Raylib, use a separate build directory and leave the graphical option disabled:

```powershell
& $cmake -S . -B build-cli -G "Visual Studio 17 2022" -A x64
& $cmake --build build-cli --config Release
& $ctest --test-dir build-cli -C Release --output-on-failure
.\build-cli\Release\slot_simulator.exe 1000000 42
```

**Important:** CMake caches the toolchain path inside each build directory. If the vcpkg location changes, reconfigure with the correct absolute `CMAKE_TOOLCHAIN_FILE` in a new build directory (recommended) or deliberately regenerate that directory. Do not assume a previously generated build directory will repair an invalid cached path by itself.

If `cmake` or `ctest` is not recognized in PowerShell, call the executable by its full path as shown above, or add the CMake `bin` directory to the Windows `PATH`.

## Build on Linux, WSL, or macOS

For a CLI-only build with CMake and a C++17 compiler:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/slot_simulator 1000000 42
```

To build the graphical application, install Raylib and ensure its CMake package is discoverable, then enable `SLOT_BUILD_GAME`:

```bash
cmake -S . -B build-raylib -DCMAKE_BUILD_TYPE=Release -DSLOT_BUILD_GAME=ON
cmake --build build-raylib --parallel
ctest --test-dir build-raylib --output-on-failure
./build-raylib/slot_game
```

## Simulator usage

```text
slot_simulator [spin_count] [seed]
```

Both arguments are optional. The default is 1,000,000 spins with seed 42.

Example on Windows:

```powershell
.\build-raylib\Release\slot_simulator.exe 100000 42
```

The payout rate is:

```text
total payout / total wagered × 100
```

It is a statistic from the configured toy simulation, not a guarantee for an individual play or a property of a real game. Runtime and throughput depend on build configuration, machine, background load, and measurement granularity; benchmark comparisons should use the same build and parameters across multiple runs.

## Tests

The `slot_engine_tests` executable currently covers:
- Matching and non-matching symbols.
- Payout scaling with the bet.
- Reproducibility when engines use the same seed.
- Simulation counters and zero-wager payout-rate handling.
- `GameSession` bet validation, balance updates, and insufficient-balance rejection.

CTest registers the test executable as one test entry, so output such as `1/1 tests passed` refers to the registered executable, not the number of assertions inside it.

For each configuration, build the test target before running CTest. For example, in a Visual Studio generator:

```powershell
& $cmake --build build-raylib --config Release
& $ctest --test-dir build-raylib -C Release --output-on-failure
```

The graphical client also needs manual validation on a desktop: verify the window remains responsive, bets can be selected, spins animate and stop, balance/payout presentation matches the session result, insufficient balance is handled, and the window closes cleanly. A successful CLI build or passing unit tests alone does not verify the GUI.

## Correctness, memory and performance tools

### AddressSanitizer and UndefinedBehaviorSanitizer

With a GCC/Clang environment that supports the sanitizers:

```bash
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DSLOT_ENABLE_SANITIZERS=ON
cmake --build build-asan --parallel
ctest --test-dir build-asan --output-on-failure
./build-asan/slot_simulator 100000 42
```

Sanitizer availability depends on the compiler and platform.

### Valgrind (Linux / WSL)

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./build-debug/slot_simulator 100000 42
```

Valgrind is intended for Linux environments, including WSL; it is not the standard choice for a native Windows MSVC build.

### Profiling

Use Visual Studio Performance Profiler on Windows, or Callgrind on Linux/WSL:

```bash
valgrind --tool=callgrind ./build/slot_simulator 1000000 42
```

Profiling tools add overhead. Do not compare their elapsed time with a normal Release build.

## Learning path

1. Start with `include/slot_engine/domain.hpp` to understand the game types.
2. Read `src/evaluator.cpp` to follow deterministic payout evaluation.
3. Read `src/game_engine.cpp` to understand random symbol generation and seeding.
4. Read `src/game_session.cpp` for bet validation, balance accounting, and spin status.
5. Read `src/simulator.cpp` and `src/main.cpp` for simulation and CLI reporting.
6. Read `src/slot_game.cpp` to see how Raylib presents a result without changing it.
7. Run the tests, then change one behavior at a time and add a test before profiling.

## Limitations

- The game uses a small set of symbols and a simple matching rule for learning purposes.
- `std::mt19937_64` is a seedable pseudo-random engine appropriate for repeatable programming exercises, not a claim of suitability for regulated gambling.
- A commercial or real-money game can require a formally specified and validated mathematical model, appropriate randomness and security design, statistical analysis, and jurisdictional certification. This repository is not designed or certified for real-money use.
