#pragma once

#include "Card.hpp"
#include "Shuffler.hpp"

namespace Poker {

namespace Constants {
inline constexpr int NUM_CARDS_IN_DECK = 52;
} // namespace Constants

using DefaultContainer = std::array<Card, Constants::NUM_CARDS_IN_DECK>;
using DefaultRNG = std::mt19937_64;
using DefaultSeeder = RandomSeeder<DefaultRNG>;
using DefaultPolicy = FisherYates;

template<
    typename Container = DefaultContainer,
    typename RNG = DefaultRNG,
    typename Seeder = DefaultSeeder,
    typename Policy = DefaultPolicy
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
        shuffle();
    }

    void shuffle() {
        m_policy(m_cards, m_rng);
    }

    std::vector<Card> cards() const {
        std::vector<Card> result;
        for (const auto& card : m_cards) {
            if (!card.is_empty()) {
                result.push_back(card);
            }
        }
        return result;
    }

    std::string to_string() const {
        std::string result;
        for (const auto& card : m_cards) {
            if (!result.empty()) {
                result += " ";
            }
            result += card.to_string();
        }
        return result;
    }

private:
    Container m_cards;
    RNG m_rng;
    Policy m_policy;
};

} // namespace Poker