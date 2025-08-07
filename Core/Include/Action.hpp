#pragma once

#include <string>
#include <variant>
#include <format>
#include "Chips.hpp"

namespace Poker {

struct Check {
    std::string to_string() const { return "Check"; }
};

struct Fold {
    std::string to_string() const { return "Fold"; }
};

struct Call {
    explicit Call(Chips chips) : m_chips(chips) {}
    Chips chips() const { return m_chips; }
    std::string to_string() const { return "Call"; }
private:
    Chips m_chips;
};

struct Bet {
    explicit Bet(Chips chips) : m_chips(chips) {}
    Chips chips() const { return m_chips; }
    std::string to_string() const { return std::format("Bet {}", m_chips.amount()); }
private:
    Chips m_chips;
};

struct Raise {
    explicit Raise(Chips chips) : m_chips(chips) {}
    Chips chips() const { return m_chips; }
    std::string to_string() const { return std::format("Raise {}", m_chips.amount()); }
private:
    Chips m_chips;
};

using Action = std::variant<Check, Fold, Call, Bet, Raise>;

} // namespace Poker