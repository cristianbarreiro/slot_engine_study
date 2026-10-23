#include "slot_engine/game_engine.hpp"
#include "slot_engine/simulator.hpp"

#include <chrono>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>

int main(int argc, char* argv[]) {
    std::uint64_t spins = 1'000'000;
    std::uint64_t seed = 42;

    try {
        if (argc > 1) {
            spins = std::stoull(argv[1]);
        }
        if (argc > 2) {
            seed = std::stoull(argv[2]);
        }
        if (argc > 3) {
            std::cerr << "Usage: slot_simulator [spins] [seed]\n";
            return 2;
        }
    } catch (const std::exception&) {
        std::cerr << "Invalid argument. Usage: slot_simulator [spins] [seed]\n";
        return 2;
    }

    const slot_engine::GameConfig config{};
    slot_engine::GameEngine engine(config, seed);

    const auto start = std::chrono::steady_clock::now();
    const slot_engine::SimulationReport report = slot_engine::run_simulation(engine, spins);
    const auto end = std::chrono::steady_clock::now();

    const double elapsed_seconds = std::chrono::duration<double>(end - start).count();
    const double spins_per_second = elapsed_seconds > 0.0
        ? static_cast<double>(report.spins) / elapsed_seconds
        : 0.0;

    std::cout << "Slot Engine Study - educational simulation\n"
              << "Seed: " << seed << '\n'
              << "Spins: " << report.spins << '\n'
              << "Winning spins: " << report.winning_spins << '\n'
              << "Total wagered (units): " << report.total_wagered << '\n'
              << "Total payout (units): " << report.total_payout << '\n'
              << std::fixed << std::setprecision(2)
              << "Payout rate: " << report.payout_rate_percent() << "%\n"
              << "Elapsed: " << elapsed_seconds << " seconds\n"
              << "Throughput: " << std::setprecision(0) << spins_per_second << " spins/second\n";

    return 0;
}
