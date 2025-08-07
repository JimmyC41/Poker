#pragma once

#include <cassert>
#include <compare>
#include <sstream>

namespace Poker {

struct Chips {
    explicit Chips(int64_t amount = 0) : m_amount(amount) {
        assert(amount >= 0);
    }

    Chips& operator+=(Chips other) {
        m_amount += other.m_amount;
        return *this;
    }

    Chips& operator-=(Chips other) {
        assert(m_amount >= other.m_amount);
        m_amount -= other.m_amount;
        return *this;
    }

    friend Chips operator+(Chips lhs, Chips rhs) {
        return lhs += rhs;
    }

    friend Chips operator-(Chips lhs, Chips rhs) {
        assert(lhs.m_amount >= rhs.m_amount);
        return lhs -= rhs;
    }

    auto operator<=>(const Chips&) const = default;

    int64_t amount() const { return m_amount; }

    std::string to_string() const {
        return std::to_string(m_amount);
    }
private:
    int64_t m_amount;
};

} // namespace Poker