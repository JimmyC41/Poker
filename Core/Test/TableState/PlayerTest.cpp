#include <gtest/gtest.h>
#include <filesystem>
#include "TableState.hpp"
#include "Parser.hpp"

using namespace ::testing;
using namespace Poker;
namespace fs = std::filesystem;

using Players = std::initializer_list<PlayerID>;
using PlayerChipsPair = std::initializer_list<std::pair<PlayerID, Chips>>;

class PlayerFixture : public Parser::FixtureBase<TableState> {
    void on_step(const Parser::Line& l, TableState& s) override {
        if (l.m_operation == "ADD") {
            ASSERT_EQ(l.m_args.size(), 2u);
            s.add_player(Parser::as_id(l.m_args[0]), Parser::as_chips(l.m_args[1]));
        }
        else if (l.m_operation == "REMOVE") {
            ASSERT_EQ(l.m_args.size(), 1u);
            s.remove_player(Parser::as_id(l.m_args[0]));
        }
        else {
            FAIL() << "Unknown LINE: " << l.m_operation;
        }
    }

    void on_expect(const Parser::Line& l, TableState& s) override {
        ASSERT_GE(l.m_args.size(), 1u) << "EXPECT <WHAT> ...";
        const auto& what = l.m_args[0];

        if (what == "IS_PLAYER") {
            ASSERT_EQ(l.m_args.size(), 3u);
            EXPECT_EQ(s.is_player(Parser::as_id(l.m_args[1])), Parser::as_bool(l.m_args[2]));
        }
        else if (what == "ADD_THROWS") {
            ASSERT_EQ(l.m_args.size(), 3u);
            EXPECT_THROW(s.add_player(Parser::as_id(l.m_args[1]), Parser::as_chips(l.m_args[2])), std::runtime_error);
        }
        else if (what == "REMOVE_THROWS") {
            ASSERT_EQ(l.m_args.size(), 2u);
            EXPECT_THROW(s.remove_player(Parser::as_id(l.m_args[1])), std::runtime_error);
        }
        else {
            FAIL() << "Unknown EXPECT: " << what;
        }
    }
};

class PlayerParamFixture : public PlayerFixture, public WithParamInterface<std::string> {};

TEST_P(PlayerParamFixture, RunScript) {
    TableState ts;
    auto steps = Parser::load_file(GetParam());
    run_script(steps, ts);
}

static const Parser::VecStr kFiles = Parser::list_files(TEST_CASES_PLAYERS); 

INSTANTIATE_TEST_SUITE_P(
    PlayerScripts,
    PlayerParamFixture,
    ValuesIn(kFiles)
);