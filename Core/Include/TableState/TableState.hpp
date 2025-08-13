#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <cstdint>
#include "Chips.hpp"
#include "Card.hpp"
#include "Deck.hpp"
#include "Pot.hpp"

namespace Poker {

using PlayerID = std::uint64_t;

namespace Constants {
inline constexpr int NUM_FLOP_CARDS = 3;
inline constexpr int MIN_PLAYERS = 2;
} // namespace Constants

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

    void on_new_wager(Chips amount);
    void on_fold();
    void on_check();
    void subtract_from_total(Chips amount);
    void reset_cur_bet();
    void reset_bets();
};

class TableState {
private:
    static constexpr int MAX_SEATS = 9;

    // Players
    std::vector<PlayerID> m_players;
    std::map<PlayerID, SeatState> m_states;
    std::optional<PlayerID> m_to_act{std::nullopt};
    std::optional<PlayerID> m_button{std::nullopt};

    // Action
    Chips m_active_bet{};
    std::vector<Pot> m_pots{}; // Calculated after bets are wagered

    // Dealing
    Deck<> m_deck;
    std::vector<Card> m_community_cards;

public:
    // Players
    void add_player(PlayerID id, Chips stack = Chips{0});
    void remove_player(PlayerID id);
    void sit_out_player(PlayerID id);
    void sit_in_player(PlayerID id);

    // Action
    void reset_all_action();
    void reset_street_action();
    bool is_action_complete() const;
    void on_player_wager(PlayerID id, Chips amount);
    void on_player_fold(PlayerID id);
    void on_player_check(PlayerID id);
    void calculate_pots();

    // Dealing
    void reset_deck();
    void deal_hole_cards();
    void deal_flop();
    void deal_turn_or_river();

    // Const Accessors
    bool can_start_hand() const;
    int num_players() const;
    Chips sum_of_stacks() const;
    Chips stack(PlayerID id) const;
    bool is_player(PlayerID id) const;

private:
    SeatState& player_state(PlayerID id);
    const SeatState& player_state(PlayerID id) const;
    bool is_in_hand(PlayerID id) const;
    bool is_live(PlayerID id) const;
    bool is_active(PlayerID id) const;
    bool has_acted(PlayerID id) const;
    bool bet_matched(PlayerID id) const;
    void assert_bets_accounted() const;
    Chips min_live_wager() const;
};

}