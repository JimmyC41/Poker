#pragma once

#include "TableState.hpp"

namespace Poker {
namespace Helper {

inline constexpr int NUM_MIN_PLAYERS = 2;
inline constexpr int NUM_HEADS_UP = 2;
inline constexpr int NUM_STREETS = 6;
inline constexpr int NUM_FLOP_CARDS = 3;
inline constexpr int NUM_TURN_CARDS = 1;
inline constexpr int NUM_RIVER_CARDS = 1;
inline constexpr int NUM_MAX_PLAYERS = 8;

inline const std::unordered_set<Street> non_action_streets = {
    Street::Waiting,
    Street::Showdown
};

inline const std::unordered_set<Street> streets_after_preflop = {
    Street::Flop,
    Street::Turn,
    Street::River,
    Street::Showdown
};

/* Invariant Helpers */

void assert_min_players_invariant(TableState& s);
void assert_action_command_invariant(TableState& s);
void assert_action_complete_invariant(TableState& s);

/* Impl Helpers - State Modifiers */

void post_blind(TableState& s, PlayerID id, std::optional<Chips> blind);
void set_target_street(TableState& s, Street target_street);
void deal_community_cards(TableState& s, int num);
void reset_action_for_new_hand(TableState& s);
void reset_action_for_new_street(TableState& s);
void assign_blinds_and_button(TableState& s);
void set_utg_to_act(TableState& s);
void set_ep_to_act(TableState& s);
void nullify_player_sitting_out(TableState& s, PlayerID id);
void nullify_to_act(TableState& s);

/* Anonymous utility helpers */

int num_sitting_in(const TableState& s);
SeatState& seat_state(PlayerID id);
bool is_player_seated(const TableState& s, PlayerID id);
bool is_action_complete(const TableState& s);
Street next_street(Street cur);
PlayerID first_sitting_in(const TableState& s);
PlayerID next_sitting_in(const TableState& s, PlayerID cur);
PlayerID next_can_act(const TableState& s, PlayerID cur);

/* Templated helpers */

template <class Pred>
PlayerID next_player_if(const TableState& s, PlayerID cur, Pred pred, std::string_view err) {
    auto it = std::find(s.m_players.begin(), s.m_players.end(), cur);
    if (it == s.m_players.end()) {
        throw std::runtime_error("Player not seated at the table");
    }

    auto idx = std::distance(s.m_players.begin(), it);
    const auto n = s.m_players.size();
    for (auto step = 1; step < n; ++step) {
        const auto next_idx = (idx + step) % n;
        const auto cand_id = s.m_players[next_idx];
        if (pred(cand_id)) {
            return cand_id;
        }
    }
    throw std::runtime_error(std::string(err));
}

} // namespace Helper
} // namespace Poker