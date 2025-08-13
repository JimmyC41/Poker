#include <ranges>
#include <iostream>
#include "TableState.hpp"

namespace Poker {

/* SeatState Struct */

void SeatState::on_new_wager(Chips amount) {
    m_stack -= amount;
    m_cur_bet += amount;
    m_total_bet += amount;
    m_has_acted = true;

    if (m_stack == Chips{0}) {
        m_all_in = true;
    }
}

void SeatState::on_fold() {
    m_cur_bet = Chips{0};
    m_folded = true;
    m_has_acted = true;
}

void SeatState::on_check() {
    m_has_acted = true;
}

void SeatState::subtract_from_total(Chips amount) {
    m_total_bet -= amount;
}

void SeatState::reset_cur_bet() {
    m_cur_bet = Chips{0};
    m_has_acted = false;
}

void SeatState::reset_bets() {
    m_cur_bet = Chips{0};
    m_total_bet = Chips{0};
    m_all_in = false;
    m_folded = false;
    m_has_acted = false;
}

/* Player Management API */

void TableState::add_player(PlayerID id, Chips stack) {
    if (m_states.contains(id)) {
        throw std::runtime_error("Player already seated");
    }
    m_players.emplace_back(id);
    m_states.emplace(id, SeatState{stack});
}

void TableState::remove_player(PlayerID id) {
    auto it = std::find(m_players.begin(), m_players.end(), id);
    if (it == m_players.end()) {
        throw std::runtime_error("Player does not exist for removal");
    }
    m_players.erase(it);
    m_states.erase(id);
}

void TableState::sit_out_player(PlayerID id) {
    auto& s = player_state(id);
    s.m_sitting_in = false;
}

void TableState::sit_in_player(PlayerID id) {
    auto& s = player_state(id);
    s.m_sitting_in = true;
}

/* Action API */

// Clean table state for new hand
void TableState::reset_all_action() {
    // Action
    m_active_bet = Chips{0};
    m_pots.clear();
    m_pots.emplace_back();

    // Players
    m_to_act = std::nullopt;
    std::for_each(
        m_states.begin(),
        m_states.end(),
        [](auto& pair) { pair.second.reset_bets(); }
    );
}

void TableState::reset_street_action() {
    // Action
    m_active_bet = Chips{0};

    // Players
    m_to_act = std::nullopt;
    std::for_each(
        m_states.begin(),
        m_states.end(),
        [](auto& pair) { pair.second.reset_cur_bet(); }
    );
}

bool TableState::is_action_complete() const {
    int in_hand = 0;
    int can_act = 0;
    int matched = 0;
    int acted = 0;
    
    for (const auto& id : m_players) {
        const auto &s = m_states.at(id);

        if (s.m_folded) continue;
        ++in_hand;

        if (s.m_all_in) continue;
        ++can_act;

        if (s.m_cur_bet == m_active_bet)
            ++matched;

        if (s.m_has_acted)
            ++acted;
    }
    
    // All but 1 player has folded
    if (in_hand <= 1) return true;

    // No more players to act (all have folded or are all in)
    if (can_act == 0) return true;

    // All players that can act must match an active bet
    if (m_active_bet > Chips{0})
        return can_act == matched;

    // If no active bet, all players that can act must check
    return can_act == acted;
}

void TableState::on_player_wager(PlayerID id, Chips amount) {
    auto& s = player_state(id);
    auto bet_delta = amount - s.m_cur_bet;

    if (amount < s.m_cur_bet) {
        throw std::runtime_error("Cannot bet less than amount already wagered");
    }
    if (bet_delta > s.m_stack) {
        throw std::runtime_error("Cannot bet amount more than stack");
    }
    
    if (amount > m_active_bet) {
        m_active_bet = amount;
    }
    s.on_new_wager(bet_delta);
}

void TableState::on_player_fold(PlayerID id) {
    auto &s = player_state(id);
    s.on_fold();
}

void TableState::on_player_check(PlayerID id) {
    auto& s = player_state(id);
    s.on_check();
}

using PlayerWagers = std::pair<PlayerID, Chips>;

void TableState::calculate_pots() {
    assert_bets_accounted();
    m_pots.clear();

    // Build sorted wagers from live players
    std::vector<PlayerWagers> wagers;
    wagers.reserve(m_states.size());
    for (const auto& [id, s] : m_states) {
        if (is_in_hand(id) && s.m_total_bet > Chips{0}) {
            wagers.emplace_back(id, s.m_total_bet);
        }
    }

    if (wagers.empty()) {
        return;
    }

    std::ranges::sort(wagers, {}, &PlayerWagers::second);

    // Build pots in layer
    Chips prev_wager{0};
    for (auto i = 0; i < wagers.size(); ) {
        const Chips cur_wager = wagers[i].second;
        const Chips increment = cur_wager - prev_wager;

        if (increment == Chips{0}) {
            break;
        }

        Pot pot{};

        // Players [i, end) are eligible for this round of contribution
        for (auto j = i; j < wagers.size(); ++j) {
            pot.add_player_contribution(wagers[j].first, increment);
        }
        
        m_pots.emplace_back(std::move(pot));
        prev_wager = cur_wager;

        // Skip players with no more chips after contributing the current layer
        while (i < wagers.size() && wagers[i].second == cur_wager) ++i;
    }

    // Zero out totals
    for (auto& [id, s] : m_states) {
        s.m_total_bet = Chips{0};
    }
}

/* Dealing API */

void TableState::reset_deck() {
    m_deck.reset();
}

void TableState::deal_flop() {
    for (auto i = 0; i < Constants::NUM_FLOP_CARDS; ++i) {
        m_community_cards.emplace_back(m_deck.deal());
    }
}

void TableState::deal_turn_or_river() {
    m_community_cards.emplace_back(m_deck.deal());
}

/* State Accessors */

int TableState::num_players() const {
    if (m_players.size() != m_states.size()) {
        throw std::logic_error("IDs in m_players and m_states is inconsistent");
    }
    return m_players.size();
}

bool TableState::can_start_hand() const {
    return num_players() >= Constants::MIN_PLAYERS;
}

Chips TableState::sum_of_stacks() const {
    auto stacks = m_states
                    | std::views::values
                    | std::views::transform(&SeatState::m_stack);
    return std::accumulate(stacks.begin(), stacks.end(), Chips{0});
}

Chips TableState::stack(PlayerID id) const {
    const auto s = player_state(id);
    return s.m_stack;
}

bool TableState::is_player(PlayerID id) const {
    return m_states.find(id) != m_states.end();
}

// Private Helpers

SeatState& TableState::player_state(PlayerID id) {
    auto it = m_states.find(id);
    if (it == m_states.end()) {
        throw std::runtime_error("Cannot find seat for given player ID");
    }
    return it->second;
}

const SeatState& TableState::player_state(PlayerID id) const {
    return const_cast<TableState*>(this)->player_state(id);
}

bool TableState::bet_matched(PlayerID id) const {
    const auto s = player_state(id);
    return s.m_cur_bet == m_active_bet;
}

// True if a player is eligible to win the pot
bool TableState::is_in_hand(PlayerID id) const {
    const auto s = player_state(id);
    return s.m_sitting_in && !s.m_folded;
}

// True if a player can act
bool TableState::can_act(PlayerID id) const {
    const auto s = player_state(id);
    return s.m_sitting_in && !s.m_folded && !s.m_all_in;
}

bool TableState::has_acted(PlayerID id) const {
    const auto s = player_state(id);
    return s.m_has_acted;
}

void TableState::assert_bets_accounted() const {
    if (std::any_of(
        m_states.begin(),
        m_states.end(),
        [](const auto& pair) {
            return pair.second.m_cur_bet != Chips{0};
        }))
    {
        throw std::runtime_error("Cannot calculate the pot due to outstanding wager");
    }
}

// Returns the smallest total bet among live players
Chips TableState::min_live_wager() const {
    auto bets = m_states
            | std::views::filter([this](const auto& kv) { return is_in_hand(kv.first); })
            | std::views::values
            | std::views::transform(&SeatState::m_total_bet);
    return std::ranges::min(bets);
}

}