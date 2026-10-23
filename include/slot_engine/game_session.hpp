#pragma once

#include "slot_engine/game_engine.hpp"

#include <cstdint>

namespace slot_engine {

enum class SessionStatus {
    Completed,
    InvalidBet,
    InsufficientBalance
};

struct SessionSpinResult {
    SessionStatus status{SessionStatus::InvalidBet};
    SpinResult spin{};
    std::uint64_t balance{0};

    bool completed() const noexcept {
        return status == SessionStatus::Completed;
    }
};

// Application-layer coordinator for a single player's balance and spin lifecycle.
class GameSession {
public:
    GameSession(GameEngine& engine, std::uint64_t initial_balance);

    bool set_bet(std::uint32_t bet_per_spin) noexcept;
    SessionSpinResult spin();

    std::uint64_t balance() const noexcept;
    std::uint32_t bet_per_spin() const noexcept;

private:
    GameEngine& engine_;
    std::uint64_t balance_;
    std::uint32_t bet_per_spin_;
};

} // namespace slot_engine
