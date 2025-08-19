#include <gtest/gtest.h>
#include <filesystem>
#include <string_view>
#include "Parser.hpp"
#include "TableState.hpp"
#include "Handler.hpp"
#include "Chips.hpp"
#include "Helper.hpp"

using namespace Poker;
using namespace ::testing;

enum class TestCommand {
    Unknown,
    StartHand,
    StartFlop,
    StartTurn,
    StartRiver,
    StartShowdown,
    EndHand,
    Check,
    Fold,
    PostSmall,
    PostBig,
    Wager,
    AddPlayer,
    RemovePlayer,
    SitIn,
    SitOut
};

static TestCommand parse_command(std::string_view cmd) noexcept {
    using enum TestCommand;

    /* Lifecycle */
    if (cmd == "START_HAND")        return StartHand;
    if (cmd == "START_FLOP")        return StartFlop;
    if (cmd == "START_TURN")        return StartTurn;
    if (cmd == "START_RIVER")       return StartRiver;
    if (cmd == "START_SHOWDOWN")    return StartShowdown;
    if (cmd == "END_HAND")          return EndHand;

    /* Player Actions */
    if (cmd == "CHECK")             return Check;
    if (cmd == "FOLD")              return Fold;
    if (cmd == "POST_SMALL")        return PostSmall;
    if (cmd == "POST_BIG")          return PostBig;
    if (cmd == "WAGER")             return Wager;

    /* Table Management */
    if (cmd == "ADD" )              return AddPlayer;
    if (cmd == "REMOVE")            return RemovePlayer;
    if (cmd == "SIT_IN")            return SitIn;
    if (cmd == "SIT_OUT")           return SitOut;

    return Unknown;
}

class TestFixture : public FixtureBase<TableState> {
    void on_step(const Line& l, TableState& s) override {
        switch (parse_command(l.operation)) {

        /* Lifecycle */
        case TestCommand::StartHand:     apply(s, StartHand{}); break;
        case TestCommand::StartFlop:     apply(s, StartFlop{}); break;
        case TestCommand::StartTurn:     apply(s, StartTurn{}); break;
        case TestCommand::StartRiver:    apply(s, StartRiver{}); break;
        case TestCommand::StartShowdown: apply(s, StartShowdown{}); break;
        case TestCommand::EndHand:       apply(s, EndHand{}); break;
        
        /* Player Actions */
        case TestCommand::Check:
            ASSERT_EQ(l.args.size(), 1u);
            apply(s, Check{as_id(l.args[0])});
            break;

        case TestCommand::Fold:
            ASSERT_EQ(l.args.size(), 1u);
            apply(s, Fold{as_id(l.args[0])});
            break;

        case TestCommand::PostSmall:
            ASSERT_EQ(l.args.size(), 1u);
            apply(s, PostSmall{as_id(l.args[0])});
            break;

        case TestCommand::PostBig:
            ASSERT_EQ(l.args.size(), 1u);
            apply(s, PostBig{as_id(l.args[0])});
            break;

        case TestCommand::Wager:
            ASSERT_EQ(l.args.size(), 2u);
            apply(s, Wager{as_id(l.args[0]), as_chips(l.args[1])});
            break;
        
        /* Table Management */

        case TestCommand::AddPlayer:
            ASSERT_EQ(l.args.size(), 2u);
            apply(s, AddPlayer{as_id(l.args[0]), as_chips(l.args[1])});
            break;

        case TestCommand::RemovePlayer:
            ASSERT_EQ(l.args.size(), 1u);
            apply(s, RemovePlayer{as_id(l.args[0])});
            break;

        case TestCommand::SitIn:
            ASSERT_EQ(l.args.size(), 1u);
            apply(s, SitIn{as_id(l.args[0])});
            break;

        case TestCommand::SitOut:
            ASSERT_EQ(l.args.size(), 1u);
            apply(s, SitOut{as_id(l.args[0])});
            break;

        case TestCommand::Unknown:
        default:
            FAIL() << "Unknown operation: " << l.operation;
            break;
        }
    }

    void on_expect(const Line& l, TableState& s) override {
        const auto& what = l.args[0];
        if (what == "ACTION_COMPLETE") {
            EXPECT_EQ(Helper::is_action_complete(s), as_bool(l.args[1]));
        }
        else if (what == "IS_PLAYER") {
            EXPECT_EQ(Helper::is_player_seated(s, as_id(l.args[1])), as_bool(l.args[2]));
        }
        else if (what == "ADD_THROWS") {
            EXPECT_THROW(apply(s, AddPlayer{as_id(l.args[1]), as_chips(l.args[2])}), std::runtime_error);
        }
        else if (what == "REMOVE_THROWS") {
            EXPECT_THROW(apply(s, RemovePlayer{as_id(l.args[1])}), std::runtime_error);
        }
        else {
            FAIL() << "Unknown expect" << what;
        }
    }
};
