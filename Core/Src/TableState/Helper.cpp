#include <unordered_set>
#include <algorithm>
#include <optional>
#include <stdexcept>
#include <format>
#include "Helper.hpp"

namespace Poker {
namespace Helper {

/* Invariants */

void assert_min_players_invariant(TableState& s) {
    if (num_sitting_in(s) < NUM_MIN_PLAYERS) {
        throw std::runtime_error(std::format("At least {} players required to start the hand", NUM_MIN_PLAYERS));
    }
}

void assert_action_command_invariant(TableState& s) {
    if (non_action_streets.contains(s.m_street)) {
        throw std::runtime_error("Action command can only be invoked during a betting street");
    }
    if (!s.m_small || !s.m_big) {
        throw std::runtime_error("Action cannot occur until the small and big blind amounts have been set!");
    }
    if (!s.m_big_id) {
        throw std::runtime_error("Action cannot occur until the big blind player has been assigned");
    }
}

void assert_action_complete_invariant(TableState& s) {
    if (!is_action_complete(s)) {
        throw std::runtime_error("Action is not complete, cannot move to the next street");
    }
}

void assert_player_exists(TableState& s) {

}

/* Table State Modifiers */

void post_blind(TableState& s, PlayerID id, std::optional<Chips> blind) {
    if (!blind) {
        throw std::runtime_error("Blind must have a value");
    }

    auto& seat = s.seat_state(id);
    seat.blind(*blind);
}

void set_target_street(TableState& s, Street target_street) {
    if (target_street != next_street(s.m_street)) {
        throw std::runtime_error("Cannot advance by more than one street");
    }
    
    if (streets_after_preflop.contains(target_street)) {
        assert_action_complete_invariant(s);
    }

    s.m_street = target_street;
}

void deal_community_cards(TableState& s, int num) {
    for (auto i = 0; i < num; ++i) {
        s.m_community_cards.emplace_back(s.m_deck.deal());
    }
}

void reset_action_for_new_hand(TableState& s) {
    // Players
    std::for_each(
        s.m_states.begin(),
        s.m_states.end(),
        [](auto& kv) { kv.second.reset_all_action(); }
    );

    // Action
    s.m_street = Street::Preflop;
    s.m_active_bet = Chips{0};
    s.m_pots.clear();
    s.m_pots.emplace_back();

    // Dealing
    s.m_deck.reset();
}

void reset_action_for_new_street(TableState& s) {
    // Players
    std::for_each(
        s.m_states.begin(),
        s.m_states.end(),
        [](auto& p) { p.second.reset_cur_bet(); }
    );

    // Action
    s.m_active_bet = Chips{0};
}

void assign_blinds_and_button(TableState& s) {
    const auto n = num_sitting_in(s);
    if (n < NUM_MIN_PLAYERS) {
        throw std::runtime_error(std::format("Cannot assign blinds and button with less than {} players sitting in", NUM_MIN_PLAYERS));
    }

    const auto prev_sb = s.m_small_id;
    const auto prev_bb = s.m_big_id;
    const auto prev_button = s.m_button;

    // No blinds or button set
    if (!prev_sb && !prev_bb && !prev_button) {
        const auto first = first_sitting_in(s);
        if (n == NUM_HEADS_UP) {
            s.m_button = first;
            s.m_small_id = first;
            s.m_big_id = next_sitting_in(s, first);
            return;
        }
        else {
            s.m_button = first;
            s.m_small_id = next_sitting_in(s, first);
            s.m_big_id = next_sitting_in(s, *s.m_small_id);
            return;
        }
    }

    // HU case
    if (n == NUM_HEADS_UP) {
        s.m_button = prev_bb;
        s.m_small_id = prev_bb;
        s.m_big_id = prev_sb;
        return;
    }

    // > 2 players, lone BB case
    // Todo: Handle case where SB and Button sit out
    if (!prev_sb && prev_bb) {
        s.m_small_id = *prev_bb;
        s.m_big_id = next_sitting_in(s, *prev_bb);
        return;
    }

    // > 2 players, both SB and BB exist
    if (prev_sb && prev_bb) {
        s.m_button = *prev_sb;
        s.m_small_id = *prev_bb;
        s.m_big_id = next_sitting_in(s, *prev_bb);
        return;
    }

    throw std::logic_error("Cannot assign blinds and button because of unexpected table state");
}

void set_utg_to_act(TableState& s) {
    if (!s.m_big_id) {
        throw std::runtime_error("Big blind player must be assigned to determine first to act preflop");
    }
    s.m_to_act = next_can_act(s, *s.m_big_id);
}

void set_ep_to_act(TableState& s) {
    if (!s.m_button) {
        throw std::runtime_error("Button player must be assigned to determine first to act");
    }
    s.m_to_act = next_can_act(s, *s.m_button);
}

void nullify_player_sitting_out(TableState& s, PlayerID id) {
    if (s.m_button == id) {
        s.m_button = std::nullopt;
    }
    if (s.m_small_id == id) {
        s.m_small_id = std::nullopt;
    }
    if (s.m_big_id == id) {
        s.m_big_id = std::nullopt;
    }
}

void nullify_to_act(TableState& s) {
    s.m_to_act = std::nullopt;
}

/* Anonymous Utility Functions */

int num_sitting_in(const TableState& s) {
    return std::count_if(
        s.m_states.begin(),
        s.m_states.end(),
        [](const auto& kv) {
            return kv.second.m_sitting_in;
        }
    );
}

bool is_player_seated(const TableState& s, PlayerID id) {
    return s.m_states.find(id) != s.m_states.end();
}

bool is_action_complete(const TableState& s) {
    int in_hand = 0;    // Not folded
    int can_act = 0;    // In hand and not all in
    int to_act = 0;     // Still owes action
    const bool active_bet = (s.m_active_bet > Chips{0});
    
    for (const auto& id : s.m_players) {
        const auto& seat = s.seat_state(id);

        if (seat.m_folded) continue;
        ++in_hand;

        if (seat.m_all_in) continue;
        ++can_act;

        if (active_bet) {
            // Owes call/raise/fold
            if (!seat.m_has_acted || seat.m_cur_bet < s.m_active_bet) ++to_act;
        }

        else {
            // Owes check/bet
            if (!seat.m_has_acted) ++to_act;
        }
    }
    
    if (in_hand <= 1)   return true; // Everyone else folded
    if (can_act == 0)   return true; // No one left who can act
    if (to_act > 0)     return false;

    // Option for BB
    if (s.m_street == Street::Preflop && active_bet && s.m_active_bet == s.m_big) {
        const auto& big_seat = s.seat_state(*s.m_big_id);
        if (!big_seat.m_has_acted) return false;
    }

    return true;
}

Street next_street(Street cur) {
    return static_cast<Street>((static_cast<int>(cur) + 1) % NUM_STREETS);
}

PlayerID first_sitting_in(const TableState& s) {
    auto n = num_sitting_in(s);
    if (n == 0) {
        throw std::runtime_error("No players sitting in");
    }

    auto it = std::find_if(
        s.m_players.begin(),
        s.m_players.end(),
        [&](PlayerID id) {
            return s.seat_state(id).m_sitting_in;
        }
    );

    if (it == s.m_players.end()) {
        throw std::logic_error("Invariant broken for first_sitting_in");
    }
    return *it;
}

PlayerID next_sitting_in(const TableState& s, PlayerID cur) {
    auto sitting_in = [&](PlayerID id) {
        return s.seat_state(id).m_sitting_in;
    };
    return next_player_if(s, cur, sitting_in, "No other sitting in player found");
}

PlayerID next_can_act(const TableState& s, PlayerID cur) {
    auto can_act = [&](PlayerID id) {
        auto& seat = s.seat_state(id);
        return seat.m_sitting_in && !seat.m_folded && !seat.m_all_in;
    };
    return next_player_if(s, cur, can_act, "No next player to act");
}

} // namespace Helper
} // namespace Poker