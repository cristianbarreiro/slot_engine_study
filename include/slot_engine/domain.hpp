#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace slot_engine {

// This is a deliberately small, educational model: three fixed reels and four symbols.
enum class Symbol : std::uint8_t {
    Cherry = 0,
    Lemon,
    Bell,
    Seven,
    Count
};

inline constexpr std::size_t kSymbolCount = static_cast<std::size_t>(Symbol::Count);
inline constexpr std::size_t kReelCount = 3;
using Reels = std::array<Symbol, kReelCount>;

constexpr std::size_t symbol_index(Symbol symbol) noexcept {
    return static_cast<std::size_t>(symbol);
}

constexpr std::string_view symbol_name(Symbol symbol) noexcept {
    switch (symbol) {
        case Symbol::Cherry: return "Cherry";
        case Symbol::Lemon:  return "Lemon";
        case Symbol::Bell:   return "Bell";
        case Symbol::Seven:  return "Seven";
        case Symbol::Count:  break;
    }
    return "Unknown";
}

struct GameConfig {
    // Payout multipliers when all three symbols match, indexed by Symbol.
    // These numbers are illustrative only, not a real game's mathematical model.
    std::array<std::uint32_t, kSymbolCount> triple_payouts{{4, 3, 5, 10}};
    std::uint32_t bet_per_spin{1};
};

struct SpinResult {
    Reels reels{};
    std::uint64_t payout{0};

    bool is_win() const noexcept {
        return payout > 0;
    }
};

} // namespace slot_engine
