#include "slot_engine/evaluator.hpp"
#include "slot_engine/game_engine.hpp"
#include "slot_engine/simulator.hpp"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

int failures = 0;

void expect(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

void test_matching_symbols_pay() {
    const slot_engine::GameConfig config{};
    const slot_engine::Reels reels{
        slot_engine::Symbol::Seven,
        slot_engine::Symbol::Seven,
        slot_engine::Symbol::Seven
    };

    const auto result = slot_engine::evaluate_spin(reels, config);
    expect(result.is_win(), "three matching symbols should win");
    expect(result.payout == 10, "three Sevens should pay 10 units with default bet");
}

void test_non_matching_symbols_do_not_pay() {
    const slot_engine::GameConfig config{};
    const slot_engine::Reels reels{
        slot_engine::Symbol::Seven,
        slot_engine::Symbol::Cherry,
        slot_engine::Symbol::Seven
    };

    const auto result = slot_engine::evaluate_spin(reels, config);
    expect(!result.is_win(), "non-matching symbols should not win");
    expect(result.payout == 0, "non-matching symbols should pay zero");
}

void test_bet_scales_payout() {
    slot_engine::GameConfig config{};
    config.bet_per_spin = 3;
    const slot_engine::Reels reels{
        slot_engine::Symbol::Bell,
        slot_engine::Symbol::Bell,
        slot_engine::Symbol::Bell
    };

    const auto result = slot_engine::evaluate_spin(reels, config);
    expect(result.payout == 15, "Bell payout should scale with the bet");
}

void test_seed_reproducibility() {
    const slot_engine::GameConfig config{};
    slot_engine::GameEngine first(config, 12345);
    slot_engine::GameEngine second(config, 12345);

    for (int i = 0; i < 100; ++i) {
        const auto a = first.spin();
        const auto b = second.spin();
        expect(a.reels == b.reels, "same seed should reproduce the same reels");
        expect(a.payout == b.payout, "same seed should reproduce the same payouts");
    }
}

void test_simulation_counters() {
    const slot_engine::GameConfig config{};
    slot_engine::GameEngine engine(config, 7);
    const auto report = slot_engine::run_simulation(engine, 10'000);

    expect(report.spins == 10'000, "report should record requested spin count");
    expect(report.total_wagered == 10'000, "default bet should wager one unit per spin");
    expect(report.winning_spins <= report.spins, "wins cannot exceed spin count");
    expect(report.payout_rate_percent() >= 0.0, "payout rate should be non-negative");
}

void test_zero_wager_is_safe() {
    slot_engine::SimulationReport report{};
    expect(report.payout_rate_percent() == 0.0, "zero wager should not divide by zero");
}

} // namespace

int main() {
    test_matching_symbols_pay();
    test_non_matching_symbols_do_not_pay();
    test_bet_scales_payout();
    test_seed_reproducibility();
    test_simulation_counters();
    test_zero_wager_is_safe();

    if (failures != 0) {
        std::cerr << failures << " test assertion(s) failed.\n";
        return EXIT_FAILURE;
    }

    std::cout << "All tests passed.\n";
    return EXIT_SUCCESS;
}
