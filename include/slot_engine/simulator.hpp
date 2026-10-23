#pragma once

#include "slot_engine/game_engine.hpp"

#include <cstdint>

namespace slot_engine {

struct SimulationReport {
    std::uint64_t spins{0};
    std::uint64_t total_wagered{0};
    std::uint64_t total_payout{0};
    std::uint64_t winning_spins{0};

    double payout_rate_percent() const noexcept;
};

SimulationReport run_simulation(GameEngine& engine, std::uint64_t spin_count);

} // namespace slot_engine
