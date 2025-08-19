#include <stdexcept>
#include <format>
#include "Handler.hpp"
#include "TableState.hpp"
#include "Chips.hpp"
#include "Helper.hpp"

namespace Poker {

namespace HandlerImpl {

/* Command Pattern: Lifecycle Handlers */

void start_hand_handler(TableState& s) {
    Helper::assert_min_players_invariant(s);
    Helper::set_target_street(s, Street::Preflop);
    Helper::reset_action_for_new_street(s);
    Helper::assign_blinds_and_button(s);
    Helper::set_utg_to_act(s);
}

void start_flop_handler(TableState& s) {
    // Check if there is action to be had!
    Helper::set_target_street(s, Street::Flop);
    Helper::reset_action_for_new_street(s);
    Helper::set_ep_to_act(s);
    Helper::deal_community_cards(s, Helper::NUM_FLOP_CARDS);
}

void start_turn_handler(TableState& s) {
    // Check if there is action to be had!
    Helper::set_target_street(s, Street::Turn);
    Helper::reset_action_for_new_street(s);
    Helper::set_ep_to_act(s);
    Helper::deal_community_cards(s, Helper::NUM_TURN_CARDS);
}

void start_river_handler(TableState& s) {
    // Check if there is action to be had!
    Helper::set_target_street(s, Street::River);
    Helper::reset_action_for_new_street(s);
    Helper::set_ep_to_act(s);
    Helper::deal_community_cards(s, Helper::NUM_RIVER_CARDS);
}

void start_showdown_handler(TableState& s) {
    // Check if there is action to be had!
    Helper::set_target_street(s, Street::Showdown);
    Helper::nullify_to_act(s);
    // Evaluate hands and settle the pot
}

void end_hand_handler(TableState& s) {
    Helper::set_target_street(s, Street::Waiting);
    Helper::reset_action_for_new_hand(s);
    // Rotate positions
}

/* Command Pattern: Action Handlers */

void check_handler(TableState& s, const Check& c) {
    // Helper::assert_action_command_invariant(s);
    auto& seat = s.seat_state(c.m_id);
    seat.act();
}

void fold_handler(TableState& s, const Fold& c) {
    // Helper::assert_action_command_invariant(s);
    auto& seat = s.seat_state(c.m_id);
    seat.fold();
}

void post_small_handler(TableState& s, const PostSmall& c) {
    // Helper::assert_action_command_invariant(s);
    Helper::post_blind(s, c.m_id, s.m_small);
}

void post_big_handler(TableState& s, const PostBig& c) {
    // Helper::assert_action_command_invariant(s);
    Helper::post_blind(s, c.m_id, s.m_big);
}

void wager_handler(TableState& s, const Wager& c) {
    // Helper::assert_action_command_invariant(s);
    auto& seat = s.seat_state(c.m_id);

    if (c.m_amount < seat.m_cur_bet) {
        throw std::runtime_error("Cannot wager less than amount already wagered");
    }

    // How much 'more' the player must commit to pot to meet his new wager
    auto chip_delta = c.m_amount - seat.m_cur_bet;
    if (chip_delta > seat.m_stack) {
        throw std::runtime_error("Cannot wage more than own stack");
    }

    if (c.m_amount > s.m_active_bet) {
        s.m_active_bet = c.m_amount;
    }

    seat.wager(chip_delta);
}

/* Command Pattern: Table Management Handlers */

void add_player_handler(TableState& s, const AddPlayer& c) {
    if (s.m_players.size() == Helper::NUM_MAX_PLAYERS) {
        throw std::runtime_error(std::format("Table capacity of {} reached", Helper::NUM_MAX_PLAYERS));
    }

    if (s.m_street != Street::Waiting) {
        throw std::runtime_error("Cannot add a player while hand is in progression");
    }

    PlayerID id = c.m_id;
    if (s.m_states.contains(id)) {
        throw std::runtime_error("Player already seated");
    }

    s.m_players.emplace_back(id);
    s.m_states.emplace(id, SeatState{c.m_stack});
}

void remove_player_handler(TableState& s, const RemovePlayer& c) {
    if (s.m_street != Street::Waiting) {
        throw std::runtime_error("Cannot remove a player while hand is in progression");
    }

    PlayerID id = c.m_id;
    auto it = std::find(s.m_players.begin(), s.m_players.end(), id);
    if (it == s.m_players.end()) {
        throw std::runtime_error("Player does not exist for removal");
    }

    s.m_players.erase(it);
    s.m_states.erase(id);
}

void sit_in_handler(TableState& s, const SitIn& c) { 
    if (s.m_street != Street::Waiting) {
        throw std::runtime_error("Cannot sit in while hand is in progression");
    }

    auto& seat = s.seat_state(c.m_id);
    if (seat.m_sitting_in) {
        throw std::runtime_error("Player is already sitting in the game");
    }

    seat.m_sitting_in = true;
}

void sit_out_handler(TableState& s, const SitOut& c) { 
    if (s.m_street != Street::Waiting) {
        throw std::runtime_error("Cannot sit out while hand is in progression");
    }

    auto& seat = s.seat_state(c.m_id);
    seat.m_sitting_in = false;

    Helper::nullify_player_sitting_out(s, c.m_id); // If player is button or blinds, set as nullopt
}

} // namespace HandlerImpl

void apply(TableState& s, const Command& c) {
    std::visit(Overload{
        [&](const StartHand& c)     { HandlerImpl::start_hand_handler(s); },
        [&](const StartFlop& c)     { HandlerImpl::start_flop_handler(s); },
        [&](const StartTurn& c)     { HandlerImpl::start_turn_handler(s); },
        [&](const StartRiver& c)    { HandlerImpl::start_river_handler(s); },
        [&](const StartShowdown& c) { HandlerImpl::start_showdown_handler(s); },
        [&](const EndHand& c)       { HandlerImpl::end_hand_handler(s); },
        [&](const Check& c)         { HandlerImpl::check_handler(s, c); },
        [&](const Fold& c)          { HandlerImpl::fold_handler(s, c); },
        [&](const PostSmall& c)     { HandlerImpl::post_small_handler(s, c); },
        [&](const PostBig& c)       { HandlerImpl::post_big_handler(s, c); },
        [&](const Wager& c)         { HandlerImpl::wager_handler(s, c); },
        [&](const AddPlayer& c)     { HandlerImpl::add_player_handler(s, c); },
        [&](const RemovePlayer& c)  { HandlerImpl::remove_player_handler(s, c); },
        [&](const SitIn& c)         { HandlerImpl::sit_in_handler(s, c); },
        [&](const SitOut& c)        { HandlerImpl::sit_out_handler(s, c); },
    }, c);
}

} // namespace Poker