#pragma once

#include "Chips.hpp"

namespace Poker {

struct Pot {
    Pot() : m_total(0) {}

    void add(Chips amount) {
        m_total += amount;
    }

    Chips total() const { return m_total; }

    void clear() { m_total = Chips{0}; }
private:
    Chips m_total;
};

} // namespace Poker