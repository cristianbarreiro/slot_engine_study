#pragma once

#include "slot_engine/domain.hpp"

namespace slot_engine {

// Pure evaluation function: same inputs always produce the same result.
// It performs no dynamic allocations and does not depend on random state.
SpinResult evaluate_spin(const Reels& reels, const GameConfig& config) noexcept;

} // namespace slot_engine
