#pragma once

#include <optional>
#include <vector>
#include "TableConfig.hpp"
#include "Card.hpp"
#include "Pot.hpp"

namespace Poker {

enum class Street {
    Preflop = 0,
    Flop,
    Turn,
    River
};

template<typename Config = CashGameConfig>
    requires ConfigConcept<Config>
struct TableState {
    static constexpr int NumPlayers = Config::num_players;
    using Blinds = typename Config::Blinds;
    using Position = typename Config::Position;
    using Board = std::vector<Card>;
    using Seat = std::optional<int>;
    using Positions = std::array<Position, NumPlayers>;
    using ChipsArray = std::array<Chips, NumPlayers>;
    using BoolArray = std::array<bool, NumPlayers>;

    Blinds blinds;
    Chips current_bet;
    Pot pot;
    Street street;
    Board board;
    Seat to_act;
    Positions positions;
    ChipsArray committed;
    ChipsArray stacks;
    BoolArray has_folded;
};

} // namespace Poker