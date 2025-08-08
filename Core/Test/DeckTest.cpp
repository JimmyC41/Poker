#include <gtest/gtest.h>
#include <algorithm>
#include <set>
#include "Deck.hpp"

using namespace Poker;

TEST(Deck, ContainsExpectedCards) {
    Deck<> deck;
    const auto& cards = deck.cards();
    std::set<Card> unique_cards(cards.begin(), cards.end());
    EXPECT_EQ(cards.size(), Constants::NUM_CARDS_IN_DECK);
    EXPECT_EQ(unique_cards.size(), Constants::NUM_CARDS_IN_DECK);
}

TEST(Deck, Shuffle) {
    Deck<> deck;
    constexpr int ITERS = 100;
    for (size_t i = 0; i < ITERS; ++i) {
        const auto& x = deck.cards();
        std::vector<Card> before(x.begin(), x.end());

        deck.shuffle();
        const auto& y = deck.cards();
        std::vector<Card> after(y.begin(), y.end());

        EXPECT_FALSE(std::equal(before.begin(), before.end(), after.begin()));
        EXPECT_TRUE(std::is_permutation(before.begin(), before.end(), after.begin()));
    }
}