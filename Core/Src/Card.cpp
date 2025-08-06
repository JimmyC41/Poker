#include <bit>
#include <cassert>

#include "Card.hpp"

namespace Poker {

std::string Card::to_string() const {
    auto bit = std::countr_zero(m_mask); // counts consequence 0 from LSB
    assert(bit < Constants::NUM_RANKS * Constants::NUM_SUITS);
    int rank = bit / Constants::NUM_SUITS;
    int suit = bit % Constants::NUM_SUITS;
    return std::string{Constants::RANK_NAMES[rank]} + Constants::SUIT_NAMES[suit];
}

} // namespace Poker