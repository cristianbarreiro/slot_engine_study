#include "slot_engine/game_engine.hpp"
#include "slot_engine/game_session.hpp"

#include <raylib.h>

#include <array>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <string>

namespace {

constexpr Color kBackground{12, 18, 30, 255};
constexpr Color kPanel{23, 32, 48, 255};
constexpr Color kPanelLight{33, 45, 64, 255};
constexpr Color kGold{247, 190, 75, 255};
constexpr Color kText{239, 244, 250, 255};
constexpr Color kMuted{143, 159, 180, 255};
constexpr Color kGreen{92, 217, 158, 255};
constexpr Color kRed{246, 112, 116, 255};
constexpr std::array<std::uint32_t, 4> kBets{{1, 2, 5, 10}};
constexpr std::array<slot_engine::Symbol, 3> kIdleSymbols{{
    slot_engine::Symbol::Cherry, slot_engine::Symbol::Bell, slot_engine::Symbol::Seven
}};
constexpr std::array<double, 3> kReelStopTimes{{0.78, 1.08, 1.38}};
constexpr double kSymbolStepSeconds = 0.075;

void draw_text_centered(Font font, const char* text, Rectangle bounds,
                        float font_size, Color color) {
    const Vector2 size = MeasureTextEx(font, text, font_size, 1.0F);
    const Vector2 position{
        bounds.x + (bounds.width - size.x) * 0.5F,
        bounds.y + (bounds.height - size.y) * 0.5F
    };
    DrawTextEx(font, text, position, font_size, 1.0F, color);
}

bool draw_button(Font font, Rectangle bounds, const char* label, bool enabled,
                 bool accent, bool selected = false) {
    const Vector2 mouse = GetMousePosition();
    const bool hovered = enabled && CheckCollisionPointRec(mouse, bounds);
    const bool pressed = hovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    Color fill = accent ? Color{199, 143, 43, 255} : kPanelLight;
    if (!enabled) {
        fill = Color{40, 47, 60, 255};
    } else if (selected) {
        fill = Color{50, 91, 91, 255};
    } else if (hovered) {
        fill = accent ? Color{226, 167, 61, 255} : Color{47, 65, 88, 255};
    }
    if (pressed) {
        fill = accent ? Color{164, 111, 34, 255} : Color{39, 54, 73, 255};
    }

    const float offset = pressed ? 2.0F : 0.0F;
    Rectangle face = bounds;
    face.y += offset;
    DrawRectangleRounded(face, 0.22F, 12, fill);
    DrawRectangleRoundedLines(face, 0.22F, 12,
                              enabled && (hovered || selected) ? kGold : Color{62, 76, 96, 255});
    draw_text_centered(font, label, face, accent ? 25.0F : 19.0F,
                       enabled ? kText : kMuted);
    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void draw_symbol(Font font, slot_engine::Symbol symbol, Rectangle area) {
    const float cx = area.x + area.width * 0.5F;
    const float cy = area.y + area.height * 0.5F;
    const float scale = area.width / 150.0F;
    switch (symbol) {
        case slot_engine::Symbol::Cherry: {
            DrawLineEx({cx, cy - 6.0F * scale}, {cx + 14.0F * scale, cy - 30.0F * scale}, 5.0F * scale,
                       Color{109, 191, 117, 255});
            DrawEllipse(static_cast<int>(cx - 18.0F * scale), static_cast<int>(cy + 10.0F * scale),
                        19.0F * scale, 20.0F * scale, Color{217, 54, 76, 255});
            DrawEllipse(static_cast<int>(cx + 17.0F * scale), static_cast<int>(cy + 10.0F * scale),
                        19.0F * scale, 20.0F * scale, Color{238, 72, 91, 255});
            DrawCircle(static_cast<int>(cx - 23.0F * scale), static_cast<int>(cy + 2.0F * scale),
                       4.0F * scale, Color{255, 184, 183, 255});
            DrawCircle(static_cast<int>(cx + 12.0F * scale), static_cast<int>(cy + 2.0F * scale),
                       4.0F * scale, Color{255, 194, 189, 255});
            DrawEllipse(static_cast<int>(cx + 12.0F * scale), static_cast<int>(cy - 30.0F * scale),
                        13.0F * scale, 6.0F * scale, Color{106, 188, 111, 255});
            break;
        }
        case slot_engine::Symbol::Lemon:
            DrawEllipse(static_cast<int>(cx), static_cast<int>(cy), 43.0F * scale, 27.0F * scale,
                        Color{248, 202, 71, 255});
            DrawEllipse(static_cast<int>(cx - 8.0F * scale), static_cast<int>(cy - 8.0F * scale),
                        22.0F * scale, 7.0F * scale, Color{255, 226, 119, 255});
            DrawPoly({cx + 40.0F * scale, cy}, 3, 7.0F * scale, 90.0F,
                     Color{237, 178, 52, 255});
            DrawPoly({cx - 40.0F * scale, cy}, 3, 7.0F * scale, -90.0F,
                     Color{237, 178, 52, 255});
            break;
        case slot_engine::Symbol::Bell:
            DrawCircle(static_cast<int>(cx), static_cast<int>(cy - 9.0F * scale), 27.0F * scale,
                       Color{239, 174, 64, 255});
            DrawRectangle(static_cast<int>(cx - 27.0F * scale), static_cast<int>(cy - 9.0F * scale),
                          static_cast<int>(54.0F * scale), static_cast<int>(31.0F * scale),
                          Color{239, 174, 64, 255});
            DrawRectangleRounded({cx - 37.0F * scale, cy + 17.0F * scale,
                                  74.0F * scale, 11.0F * scale},
                                 0.7F, 8, Color{255, 207, 96, 255});
            DrawCircle(static_cast<int>(cx), static_cast<int>(cy + 34.0F * scale), 8.0F * scale,
                       Color{255, 207, 96, 255});
            DrawLineEx({cx - 13.0F * scale, cy - 5.0F * scale},
                       {cx - 13.0F * scale, cy + 10.0F * scale}, 4.0F * scale,
                       Color{255, 218, 130, 255});
            break;
        case slot_engine::Symbol::Seven:
            draw_text_centered(font, "7", area, area.height * 0.78F, Color{232, 83, 102, 255});
            break;
        case slot_engine::Symbol::Count:
            break;
    }
}

slot_engine::Symbol symbol_at_step(std::uint64_t step, int offset) {
    const int count = static_cast<int>(slot_engine::kSymbolCount);
    const int index = static_cast<int>(step % static_cast<std::uint64_t>(count));
    return static_cast<slot_engine::Symbol>((index + offset + count) % count);
}

} // namespace

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1120, 800, "Lucky Three | Slot Engine Study");
    if (!IsWindowReady()) {
        return 1;
    }
    SetWindowMinSize(760, 620);
    SetTargetFPS(60);

    Font font = GetFontDefault();
    const unsigned int default_font_texture = font.texture.id;
    bool owns_font = false;
    if (FileExists("assets/ui/slot-display.ttf")) {
        const Font loaded_font = LoadFontEx("assets/ui/slot-display.ttf", 40, nullptr, 0);
        if (IsFontValid(loaded_font) && loaded_font.texture.id != default_font_texture) {
            font = loaded_font;
            owns_font = true;
        }
    }

    slot_engine::GameConfig config{};
    slot_engine::GameEngine engine(config, 42);
    slot_engine::GameSession session(engine, 100);
    slot_engine::SessionSpinResult last_result{};
    bool has_result = false;
    bool spinning = false;
    double spin_started = 0.0;
    std::string message = "Choose your bet and spin";
    Color message_color = kMuted;

    while (!WindowShouldClose()) {
        const double now = GetTime();
        const double spin_elapsed = now - spin_started;
        if (spinning && spin_elapsed >= kReelStopTimes.back() + 0.16) {
            spinning = false;
            message = last_result.spin.is_win() ? "NICE HIT! YOU WON" : "No win this time - try again";
            message_color = last_result.spin.is_win() ? kGreen : kMuted;
        }

        const int screen_width = GetScreenWidth();
        const int screen_height = GetScreenHeight();
        const float center_x = static_cast<float>(screen_width) * 0.5F;
        const float scale = std::min(static_cast<float>(screen_width) / 1120.0F,
                                     static_cast<float>(screen_height) / 800.0F);
        const float panel_width = static_cast<float>(screen_width) * 0.84F;
        const float panel_left = center_x - panel_width * 0.5F;
        const float top = 26.0F * scale;

        BeginDrawing();
        ClearBackground(kBackground);
        DrawCircleGradient(
            Vector2{
                static_cast<float>(screen_width) * 0.5F,
                static_cast<float>(screen_height) / 3.0F
            },
            360.0F * scale,
            Color{28, 45, 64, 100},
            kBackground
        );
        DrawTextEx(font, "LUCKY THREE", {panel_left, top}, 30.0F * scale, 1.6F * scale, kGold);
        DrawTextEx(font, "ARCADE SLOT  /  C++17 STUDY GAME",
                   {panel_left, top + 39.0F * scale}, 13.0F * scale, 1.2F * scale, kMuted);

        const float card_y = top + 82.0F * scale;
        const float card_gap = 12.0F * scale;
        const float card_width = (panel_width - 2.0F * card_gap) / 3.0F;
        const std::array<const char*, 3> card_labels{{"CREDITS", "BET / SPIN", "LAST WIN"}};
        std::array<std::array<char, 32>, 3> card_values{};
        std::snprintf(card_values[0].data(), card_values[0].size(), "%llu",
                      static_cast<unsigned long long>(session.balance()));
        std::snprintf(card_values[1].data(), card_values[1].size(), "%u",
                      static_cast<unsigned int>(session.bet_per_spin()));
        if (has_result && !spinning) {
            std::snprintf(card_values[2].data(), card_values[2].size(), "%llu",
                          static_cast<unsigned long long>(last_result.spin.payout));
        } else {
            std::snprintf(card_values[2].data(), card_values[2].size(), "--");
        }
        for (std::size_t i = 0; i < card_labels.size(); ++i) {
            const Rectangle card{panel_left + static_cast<float>(i) * (card_width + card_gap),
                                 card_y, card_width, 84.0F * scale};
            DrawRectangleRounded(card, 0.14F, 12, kPanel);
            DrawTextEx(font, card_labels[i], {card.x + 17.0F * scale, card.y + 13.0F * scale},
                       12.0F * scale, 1.1F * scale, kMuted);
            DrawTextEx(font, card_values[i].data(), {card.x + 17.0F * scale, card.y + 35.0F * scale},
                       27.0F * scale, 0.8F * scale,
                       i == 2 && has_result && last_result.spin.is_win() ? kGreen : kText);
        }

        const float reel_top = card_y + 111.0F * scale;
        const float reel_height = static_cast<float>(screen_height) * 0.32F;
        const float reel_gap = 12.0F * scale;
        const float reel_width = (panel_width - 2.0F * reel_gap) / 3.0F;
        for (std::size_t i = 0; i < slot_engine::kReelCount; ++i) {
            const Rectangle reel{panel_left + static_cast<float>(i) * (reel_width + reel_gap),
                                 reel_top, reel_width, reel_height};
            DrawRectangleRounded({reel.x + 4.0F * scale, reel.y + 7.0F * scale,
                                  reel.width, reel.height}, 0.12F, 14, Color{6, 10, 18, 150});
            DrawRectangleRounded(reel, 0.12F, 14, Color{246, 248, 251, 255});
            DrawRectangleRoundedLines(reel, 0.12F, 14, kGold);
            const bool reel_moving = spinning && spin_elapsed < kReelStopTimes[i];
            if (reel_moving) {
                const double phase = (spin_elapsed + static_cast<double>(i) * 0.19)
                                   / kSymbolStepSeconds;
                const auto current_step = static_cast<std::uint64_t>(phase);
                const float fraction = static_cast<float>(phase - static_cast<double>(current_step));
                const float row_step = reel.height * 0.78F;
                const float symbol_height = reel.height * 0.82F;
                const float center_y = reel.y + reel.height * 0.5F - fraction * row_step;
                const int clip_x = static_cast<int>(reel.x + 3.0F * scale);
                const int clip_y = static_cast<int>(reel.y + 3.0F * scale);
                const int clip_width = static_cast<int>(reel.width - 6.0F * scale);
                const int clip_height = static_cast<int>(reel.height - 6.0F * scale);
                BeginScissorMode(clip_x, clip_y, clip_width, clip_height);
                for (int row = -1; row <= 1; ++row) {
                    const float symbol_y = center_y + static_cast<float>(row) * row_step;
                    const Rectangle moving_symbol{reel.x + 4.0F * scale,
                                                  symbol_y - symbol_height * 0.5F,
                                                  reel.width - 8.0F * scale, symbol_height};
                    draw_symbol(font, symbol_at_step(current_step, row), moving_symbol);
                }
                EndScissorMode();
                DrawRectangleRounded({reel.x, reel.y + reel.height - 7.0F * scale,
                                      reel.width * static_cast<float>(spin_elapsed / kReelStopTimes[i]),
                                      7.0F * scale}, 0.8F, 8, kGold);
            } else {
                const slot_engine::Symbol visible = has_result
                    ? last_result.spin.reels[i]
                    : kIdleSymbols[i];
                const Rectangle icon_area{reel.x + 4.0F * scale, reel.y + 4.0F * scale,
                                          reel.width - 8.0F * scale, reel.height - 8.0F * scale};
                draw_symbol(font, visible, icon_area);
            }
        }

        const float controls_y = reel_top + reel_height + 19.0F * scale;
        const float bet_width = 66.0F * scale;
        const float bet_gap = 8.0F * scale;
        DrawTextEx(font, "SELECT BET", {panel_left, controls_y + 13.0F * scale},
                   12.0F * scale, 1.0F * scale, kMuted);
        const float bets_x = panel_left + 94.0F * scale;
        for (std::size_t i = 0; i < kBets.size(); ++i) {
            const float x = bets_x + static_cast<float>(i) * (bet_width + bet_gap);
            std::array<char, 12> label{};
            std::snprintf(label.data(), label.size(), "%u", static_cast<unsigned int>(kBets[i]));
            const bool affordable = static_cast<std::uint64_t>(kBets[i]) <= session.balance();
            if (!spinning && draw_button(font, {x, controls_y, bet_width, 44.0F * scale},
                                         label.data(), affordable, false,
                                         session.bet_per_spin() == kBets[i])) {
                if (!session.set_bet(kBets[i])) {
                    message = "That bet is above your available credits";
                    message_color = kRed;
                } else {
                    message = "Bet set to " + std::to_string(kBets[i]) + " credits";
                    message_color = kMuted;
                }
            } else if (spinning) {
                draw_button(font, {x, controls_y, bet_width, 44.0F * scale}, label.data(),
                            false, false, session.bet_per_spin() == kBets[i]);
            }
        }

        const float spin_x = panel_left + panel_width - 176.0F * scale;
        const bool can_spin = !spinning && session.balance() >= session.bet_per_spin()
                           && session.bet_per_spin() > 0;
        if (!spinning && !can_spin) {
            message = "Not enough credits for this bet";
            message_color = kRed;
        }
        if (draw_button(font, {spin_x, controls_y - 3.0F * scale, 176.0F * scale, 51.0F * scale},
                        spinning ? "SPINNING..." : "SPIN", can_spin, true)) {
            last_result = session.spin();
            has_result = last_result.completed();
            if (has_result) {
                spinning = true;
                spin_started = now;
                message = "Good luck - reels are rolling";
                message_color = kGold;
            } else if (last_result.status == slot_engine::SessionStatus::InsufficientBalance) {
                message = "Not enough credits for this bet";
                message_color = kRed;
            } else {
                message = "Choose a valid bet to spin";
                message_color = kRed;
            }
        }

        const Rectangle message_box{panel_left, controls_y + 61.0F * scale,
                                    panel_width, 32.0F * scale};
        draw_text_centered(font, message.c_str(), message_box, 15.0F * scale, message_color);
        const Rectangle footer{panel_left, static_cast<float>(screen_height) - 31.0F * scale,
                               panel_width, 20.0F * scale};
        draw_text_centered(font, "MATCH 3 SYMBOLS TO WIN   |   EDUCATIONAL SIMULATION ONLY",
                           footer, 11.0F * scale, kMuted);
        // Raylib finalizes each frame (including buffer swap, input polling,
        // and target-FPS pacing) through EndDrawing in its default build.
        EndDrawing();
    }

    if (owns_font) {
        UnloadFont(font);
    }
    CloseWindow();
    return 0;
}
