#include <gtest/gtest.h>
#include "TableState.hpp"

using namespace Poker;
using namespace ::testing;

using Players = std::initializer_list<PlayerID>;
using PlayerChipsPair = std::initializer_list<std::pair<PlayerID, Chips>>;

struct PlayerFixture : testing::Test {
    TableState ts;

    void add_players(PlayerChipsPair p) {
        for (auto [id, buyin] : p)
            ts.add_player(id, buyin);
    }
    
    void apply_wagers(PlayerChipsPair p) {
        for (auto [id, wager] : p)
            ts.on_player_wager(id, wager);
    }

    void apply_checks(Players p) {
        for (auto id : p)
            ts.on_player_check(id);
    }

    void remove_players(Players p) {
        for (auto id : p)
            ts.remove_player(id);
    }

    void TEST_IS_PLAYER(Players p) {
        for (auto id : p)
            EXPECT_TRUE(ts.is_player(id));
    }

    void TEST_STACKS(PlayerChipsPair p) {
        for (auto [id, chips] : p)
            EXPECT_EQ(ts.stack(id), chips);
    }
};

TEST_F(PlayerFixture, AddPlayerThrows) {
    ts.add_player(1, Chips{10});
    EXPECT_THROW(ts.add_player(1, Chips{10}), std::runtime_error);
}

TEST_F(PlayerFixture, AddPlayer) {
    add_players({{1, Chips{10}}, {2, Chips{15}}});
    EXPECT_TRUE(ts.can_start_hand());

    add_players({{3, Chips{20}}, {4, Chips{25}}});
    TEST_IS_PLAYER({1, 2, 3, 4});
    TEST_STACKS({{1, Chips{10}}, {2, Chips{15}}, {3, Chips{20}}, {4, Chips{25}}});
}

TEST_F(PlayerFixture, RemovePlayerThrows) {
    add_players({{1, Chips{1}}, {2, Chips{1}}});
    EXPECT_THROW(ts.remove_player(100), std::runtime_error);
}

TEST_F(PlayerFixture, RemovePlayer) {
    add_players({{1, Chips{1}}, {2, Chips{1}}});
    ts.remove_player(2);
    EXPECT_FALSE(ts.can_start_hand());

    add_players({{3, Chips{1}}, {4, Chips{1}}, {5, Chips{1}}});
    remove_players({4, 5});
    TEST_IS_PLAYER({1, 3});
}