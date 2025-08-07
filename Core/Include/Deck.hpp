#pragma once

#include "Card.hpp"
#include "Shuffler.hpp"

namespace Poker {

namespace Constants {

inline constexpr int NUM_CARDS_IN_DECK = 52;

} // namespace Constants

template<
    typename Container = std::array<Card, Constants::NUM_CARDS_IN_DECK>,
    typename RNG = std::mt19937_64,
    typename Seeder = RandomSeeder<RNG>,
    typename Policy = FisherYates
>
    requires DeckConcept<Container, RNG, Seeder, Policy>
class Deck {
public:
    Deck() 
        : m_rng{Seeder{}()}
        , m_policy{}
    {
        int idx = 0;
        for (int s = 0; s < Constants::NUM_SUITS; ++s) {
            for (int r = 0; r < Constants::NUM_RANKS; ++r) {
                m_cards[idx++] = Card(static_cast<Card::Rank>(r), static_cast<Card::Suit>(s));
            }
        }
    }

    void shuffle() {
        m_policy(m_cards, m_rng);
    }

    const Container& cards() const {
        return m_cards;
    }

private:
    Container m_cards;
    RNG m_rng;
    Policy m_policy;
};

} // namespace Poker