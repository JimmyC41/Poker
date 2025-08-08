#include <gtest/gtest.h>
#include "Card.hpp"

using namespace Poker;
using Poker::Card;
using CardInfo = std::tuple<Card::Rank, Card::Suit, int, std::string>;

TEST(EmptyCard, Accessors) {
    Card default_ctor{};
    EXPECT_TRUE(default_ctor.is_empty());
    EXPECT_EQ(default_ctor.mask(), static_cast<uint64_t>(0));
    EXPECT_EQ(default_ctor.to_string(), "--");
    std::ostringstream os;
    os << default_ctor;
    EXPECT_EQ(os.str(), "--");
};

class NonEmptyCard : public ::testing::TestWithParam<CardInfo> {
    // No SetUp() or TearDown()
};

TEST_P(NonEmptyCard, Accessors) {
    auto [rank, suit, bit, str] = GetParam();
    Card c{rank, suit};

    EXPECT_EQ(c.mask(), uint64_t{1} << bit);
    EXPECT_EQ(c.to_string(), str);
    std::ostringstream os;
    os << c;
    EXPECT_EQ(os.str(), str);
}

INSTANTIATE_TEST_SUITE_P(
    NonEmptyCardSuite,
    NonEmptyCard,
    ::testing::Values(
        std::make_tuple(Card::Rank::Two,   Card::Suit::Clubs,    0,  "2C"),
        std::make_tuple(Card::Rank::Two,   Card::Suit::Spades,   1,  "2S"),
        std::make_tuple(Card::Rank::Three, Card::Suit::Diamonds, 6,  "3D"),
        std::make_tuple(Card::Rank::Ten,   Card::Suit::Hearts,   35, "10H"),
        std::make_tuple(Card::Rank::Jack,  Card::Suit::Clubs,    36,  "JC"),
        std::make_tuple(Card::Rank::Queen, Card::Suit::Spades,   41, "QS"),
        std::make_tuple(Card::Rank::King,  Card::Suit::Diamonds, 46, "KD"),
        std::make_tuple(Card::Rank::Ace,   Card::Suit::Hearts,   51, "AH")
    )
);