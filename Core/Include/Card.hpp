#pragma once

#include <cstdint>
#include <string>
#include <array>
#include <ostream>

namespace Poker {

namespace Constants {

inline constexpr int NUM_SUITS = 4; // Note to self: equivalent to extern inline constexpr
inline constexpr int NUM_RANKS = 13;

inline constexpr std::array<const char*, NUM_RANKS> RANK_NAMES = {
    "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"
};
inline constexpr std::array<const char*, NUM_SUITS> SUIT_NAMES = {
    "C", "S", "D", "H"
};
} // namespace Constants

class Card {
public:
    enum class Rank : uint8_t {
        Two = 0, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace
    };

    enum class Suit : uint8_t {
        Clubs = 0, Spades, Diamonds, Hearts
    };

    Card() = default;

    explicit Card(Rank r, Suit s) noexcept
        : m_mask{uint64_t{1} << bit_index(r, s)} {}
    
    uint64_t mask() const noexcept { return m_mask; }

    std::string to_string() const;

    friend bool operator==(const Card& lhs, const Card& rhs) {
        return lhs.mask() == rhs.mask();
    }

    friend bool operator<(const Card& lhs, const Card& rhs) {
        return lhs.mask() < rhs.mask();
    }

    friend std::ostream& operator<<(std::ostream& os, const Card& c) {
        return os << c.to_string();
    }
private:
    uint64_t m_mask;

    // Assumes rank-major layout e.g. 2C, 2S, 2D, 2H, 3C, 3S, ...
    static constexpr int bit_index(Rank r, Suit s) noexcept {
        return static_cast<int>(r) * Constants::NUM_SUITS + static_cast<int>(s);
    }
};
} // namespace Poker
