#include "TableState.hpp"

namespace Poker {

// Seat State

void SeatState::act() {
    m_has_acted = true;
}

void SeatState::fold() {
    m_has_acted = true;
    m_folded = true;
    m_cur_bet = Chips{0};
}

void SeatState::blind(Chips amount) {
    m_stack -= amount;
    m_cur_bet += amount;
    m_total_bet += amount;

    if (m_stack == Chips {0}) {
        m_all_in = true;
    }
}

void SeatState::wager(Chips amount) {
    blind(amount);
    m_has_acted = true;
}

void SeatState::reset_cur_bet() {
    m_cur_bet = Chips{0};
    m_has_acted = false;
}

void SeatState::reset_all_action() {
    m_cur_bet = Chips{0};
    m_total_bet = Chips{0};
    m_all_in = false;
    m_folded = false;
    m_has_acted = false;
}

// Seat Accessor

SeatState& TableState::seat_state(PlayerID id) {
    auto it = m_states.find(id);
    if (it == m_states.end()) {
        throw std::runtime_error("Cannot find seat state");
    }
    return it->second;
}

const SeatState& TableState::seat_state(PlayerID id) const {
    return const_cast<TableState*>(this)->seat_state(id);
}

} // namespace Poker