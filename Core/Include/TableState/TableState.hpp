#pragma once

#include <vector>
#include <map>
#include <optional>
#include "Chips.hpp"
#include "Card.hpp"
#include "Deck.hpp"
#include "Pot.hpp"

namespace Poker {

inline constexpr int NUM_STREETS = 6;

enum class Street {
    Waiting, // No hand in progress
    Preflop,
    Flop,
    Turn,
    River,
    Showdown
};

using PlayerID = std::uint64_t;
struct SeatState;

struct TableState {
    explicit TableState(Chips small, Chips big)
        : m_small{small}
        , m_big{big} {}
    
    // Player management
    std::vector<PlayerID> m_players;
    std::map<PlayerID, SeatState> m_states;
    std::optional<PlayerID> m_small_id{std::nullopt};
    std::optional<PlayerID> m_big_id{std::nullopt};
    std::optional<PlayerID> m_to_act{std::nullopt};
    std::optional<PlayerID> m_button{std::nullopt};

    // Betting action
    Street m_street{Street::Waiting};
    Chips m_active_bet{};
    std::optional<Chips> m_small{std::nullopt};
    std::optional<Chips> m_big{std::nullopt};
    std::vector<Pot> m_pots{}; // Calculated after bets are wagered
    
    // Dealt cards
    Deck<> m_deck;
    std::vector<Card> m_community_cards;

    // Seat accessor
    SeatState& seat_state(PlayerID id);
    const SeatState& seat_state(PlayerID id) const;
};

struct SeatState {
    Chips m_stack{0};
    Chips m_cur_bet{0};
    Chips m_total_bet{0};
    bool m_sitting_in{true};
    bool m_all_in{false};
    bool m_folded{false};
    bool m_has_acted{false};
    std::vector<Card> m_hole_cards{};
    SeatState(Chips stack) : m_stack(stack) {}

    void act();
    void fold();
    void wager(Chips amount);
    void blind(Chips amount);

    void reset_all_action();
    void reset_cur_bet();
};

} // namespace Poker