#include "slot_engine/evaluator.hpp"

namespace slot_engine {

SpinResult evaluate_spin(const Reels& reels, const GameConfig& config) noexcept {
    SpinResult result{};
    result.reels = reels;

    const bool all_match = reels[0] == reels[1] && reels[1] == reels[2];
    if (!all_match) {
        return result;
    }

    const std::size_t index = symbol_index(reels[0]);
    if (index >= config.triple_payouts.size()) {
        // Defensive guard in case an invalid enum value enters from external data.
        return result;
    }

    result.payout = static_cast<std::uint64_t>(config.triple_payouts[index])
                  * static_cast<std::uint64_t>(config.bet_per_spin);
    return result;
}

} // namespace slot_engine
