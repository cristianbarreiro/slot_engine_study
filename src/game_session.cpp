#include "slot_engine/game_session.hpp"

namespace slot_engine {

GameSession::GameSession(GameEngine& engine, std::uint64_t initial_balance)
    : engine_(engine),
      balance_(initial_balance),
      bet_per_spin_(engine.bet_per_spin()) {}

bool GameSession::set_bet(std::uint32_t bet_per_spin) noexcept {
    if (bet_per_spin == 0 || static_cast<std::uint64_t>(bet_per_spin) > balance_) {
        return false;
    }

    bet_per_spin_ = bet_per_spin;
    return true;
}

SessionSpinResult GameSession::spin() {
    if (bet_per_spin_ == 0) {
        return {SessionStatus::InvalidBet, {}, balance_};
    }
    if (static_cast<std::uint64_t>(bet_per_spin_) > balance_) {
        return {SessionStatus::InsufficientBalance, {}, balance_};
    }

    balance_ -= static_cast<std::uint64_t>(bet_per_spin_);
    const SpinResult result = engine_.spin(bet_per_spin_);
    balance_ += result.payout;
    return {SessionStatus::Completed, result, balance_};
}

std::uint64_t GameSession::balance() const noexcept {
    return balance_;
}

std::uint32_t GameSession::bet_per_spin() const noexcept {
    return bet_per_spin_;
}

} // namespace slot_engine
