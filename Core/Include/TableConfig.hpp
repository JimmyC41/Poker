#pragma once

#include <concepts>
#include <array>

namespace Poker {

template<typename T>
concept ConfigConcept = requires {
    { T::num_players } -> std::convertible_to<std::size_t>;

    typename T::Position;
    typename T::Blinds;

    { T::position_order } -> std::same_as<const std::array<typename T::Position, T::num_players>&>;
};

struct CashGameConfig {
    static constexpr int num_players = 8;

    enum class Position {
        SB = 0, BB, UTG, UTG1, LJ, HJ, CO, Button
    };

    struct Blinds {
        static constexpr int small_blind = 1;
        static constexpr int big_blind = 2;
    };

    static constexpr std::array<Position, num_players> position_order{
        Position::SB,
        Position::BB,
        Position::UTG,
        Position::UTG1,
        Position::LJ,
        Position::HJ,
        Position::CO,
        Position::Button
    };
};

} // namespace Poker