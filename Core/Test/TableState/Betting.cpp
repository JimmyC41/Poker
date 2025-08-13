#include <gtest/gtest.h>
#include <filesystem>
#include "TableState.hpp"
#include "Parser.hpp"

using namespace ::testing;
using namespace Poker;
namespace fs = std::filesystem;

static inline PlayerID as_id(const std::string& s) {
    return static_cast<PlayerID>(std::stoll(s));
}

static inline Chips as_chips(const std::string& s) {
    return Chips{static_cast<std::int64_t>(std::stoll(s))};
}

static inline bool as_bool(const std::string& s) {
    return std::stoi(s) != 0;
}

class BettingFixture : public Parser::FixtureBase<TableState> {
protected:
    void on_step(const Parser::Line& l, TableState& s) override {
        if (l.m_operation == "ADD") {
            s.add_player(as_id(l.m_args[0]), as_chips(l.m_args[1]));
        }
        else if (l.m_operation == "WAGER") {
            ASSERT_EQ(l.m_args.size(), 2u);
            s.on_player_wager(as_id(l.m_args[0]), as_chips(l.m_args[1]));
        }
        else if (l.m_operation == "CHECK") {
            ASSERT_EQ(l.m_args.size(), 1u);
            s.on_player_check(as_id(l.m_args[0]));
        }
        else if (l.m_operation == "FOLD") {
            ASSERT_EQ(l.m_args.size(), 1u);
            s.on_player_fold(as_id(l.m_args[0]));
        }
        else {
            FAIL() << "Unknown LINE: " << l.m_operation;
        }
    }

    void on_expect(const Parser::Line& l, TableState& s) override {
        ASSERT_GE(l.m_args.size(), 1u) << "EXPECT <WHAT> ...";
        const auto& what = l.m_args[0];

        if (what == "ACTION_COMPLETE") {
            ASSERT_EQ(l.m_args.size(), 2u);
            EXPECT_EQ(s.is_action_complete(), as_bool(l.m_args[1]));
        }
        else {
            FAIL() << "Unknown EXPECT: " << what;
        }
    }
};

class BettingParamFixture : public BettingFixture, public WithParamInterface<std::string> {};

TEST_P(BettingParamFixture, RunScript) {
    TableState ts;
    auto steps = Parser::load_file(GetParam());
    run_script(steps, ts);
}

static const Parser::VecStr kFiles = Parser::list_files(TEST_CASES_BETTING_SCENARIOS); 

INSTANTIATE_TEST_SUITE_P(
    BettingScripts,
    BettingParamFixture,
    ValuesIn(kFiles)
);

