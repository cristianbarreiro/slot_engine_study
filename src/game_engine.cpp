#include "slot_engine/game_engine.hpp"

#include "slot_engine/evaluator.hpp"

namespace slot_engine {

GameEngine::GameEngine(GameConfig config, std::uint64_t seed)
    : config_(config),
      random_engine_(seed),
      symbol_distribution_(0, kSymbolCount - 1) {}

SpinResult GameEngine::spin() {
    Reels reels{};
    for (Symbol& symbol : reels) {
        symbol = static_cast<Symbol>(symbol_distribution_(random_engine_));
    }

    return evaluate_spin(reels, config_);
}

std::uint32_t GameEngine::bet_per_spin() const noexcept {
    return config_.bet_per_spin;
}

} // namespace slot_engine
