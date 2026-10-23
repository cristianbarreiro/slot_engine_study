#pragma once

#include "slot_engine/domain.hpp"

#include <cstdint>
#include <random>

namespace slot_engine {

class GameEngine {
public:
    explicit GameEngine(GameConfig config, std::uint64_t seed);

    SpinResult spin();
    SpinResult spin(std::uint32_t bet_per_spin);
    std::uint32_t bet_per_spin() const noexcept;

private:
    GameConfig config_;
    std::mt19937_64 random_engine_;
    std::uniform_int_distribution<std::size_t> symbol_distribution_;
};

} // namespace slot_engine
