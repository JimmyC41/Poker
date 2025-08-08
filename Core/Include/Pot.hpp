#pragma once

#include <unordered_set>
#include "Chips.hpp"
#include "Player.hpp"

namespace Poker {

using PlayerSet = std::unordered_set<PlayerId>;

struct Pot {
    constexpr Pot() : m_total(0) {}

    Chips total() const { return m_total; }

    bool is_eligible(PlayerId id) const { return m_players.contains(id); }

    const PlayerSet& get_eligible() const { return m_players; }

    void add_chips(Chips amount) { m_total += amount; }

    void clear() { m_total = Chips{0}; }

    void add_player(PlayerId id) { m_players.insert(id); }

    void remove_player(PlayerId id) { m_players.erase(id); }

    void clear_players() { m_players.clear(); }
private:
    Chips m_total;
    PlayerSet m_players;
};

} // namespace Poker