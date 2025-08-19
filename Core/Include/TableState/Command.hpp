#pragma once

#include <variant>
#include <cstdint>
#include "Chips.hpp"

namespace Poker {

using Playerm_id = std::uint64_t;

// Lifecycle
struct StartHand {};
struct StartFlop {};
struct StartTurn {};
struct StartRiver {};
struct StartShowdown {};
struct EndHand {};

// Player actions
struct Check     { Playerm_id m_id; };
struct Fold      { Playerm_id m_id; };
struct PostSmall { Playerm_id m_id; };
struct PostBig   { Playerm_id m_id; };
struct Wager     { Playerm_id m_id; Chips m_amount; };

// Table management
struct AddPlayer { Playerm_id m_id; Chips m_stack; };
struct RemovePlayer { Playerm_id m_id; };
struct SitIn     { Playerm_id m_id; };
struct SitOut    { Playerm_id m_id; };

// Value based commands
using Command = std::variant<
    StartHand,
    StartFlop,
    StartTurn,
    StartRiver,
    StartShowdown,
    EndHand,
    Check,
    Fold,
    PostSmall,
    PostBig,
    Wager,
    AddPlayer,
    RemovePlayer,
    SitIn,
    SitOut
>;

} // namespace Poker