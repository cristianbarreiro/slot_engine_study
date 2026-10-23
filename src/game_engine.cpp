#include "slot_engine/game_engine.hpp"

#include "slot_engine/evaluator.hpp"

namespace slot_engine {

GameEngine::GameEngine(GameConfig config, std::uint64_t seed)
    : config_(config),
      random_engine_(seed),
      symbol_distribution_(0, kSymbolCount - 1) {}

SpinResult GameEngine::spin() {
    return spin(config_.bet_per_spin);
}

SpinResult GameEngine::spin(std::uint32_t bet_per_spin) {
    Reels reels{};
    for (Symbol& symbol : reels) {
        symbol = static_cast<Symbol>(symbol_distribution_(random_engine_));
    }

    GameConfig spin_config = config_;
    spin_config.bet_per_spin = bet_per_spin;
    return evaluate_spin(reels, spin_config);
}

std::uint32_t GameEngine::bet_per_spin() const noexcept {
    return config_.bet_per_spin;
}

} // namespace slot_engine
