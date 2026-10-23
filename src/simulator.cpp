#include "slot_engine/simulator.hpp"

namespace slot_engine {

double SimulationReport::payout_rate_percent() const noexcept {
    if (total_wagered == 0) {
        return 0.0;
    }
    return (static_cast<double>(total_payout) / static_cast<double>(total_wagered)) * 100.0;
}

SimulationReport run_simulation(GameEngine& engine, std::uint64_t spin_count) {
    SimulationReport report{};
    report.spins = spin_count;
    report.total_wagered = spin_count * static_cast<std::uint64_t>(engine.bet_per_spin());

    for (std::uint64_t i = 0; i < spin_count; ++i) {
        const SpinResult result = engine.spin();
        report.total_payout += result.payout;
        if (result.is_win()) {
            ++report.winning_spins;
        }
    }

    return report;
}

} // namespace slot_engine
