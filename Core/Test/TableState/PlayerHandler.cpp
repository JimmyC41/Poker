#include <gtest/gtest.h>
#include <filesystem>
#include "Parser.hpp"
#include "TestFixture.hpp"
#include "TableState.hpp"
#include "Handler.hpp"
#include "Chips.hpp"

using namespace ::testing;
static std::vector<std::string> k_files = list_files(TEST_CASES_PLAYERS);
class PlayerParamFixture : public TestFixture, public WithParamInterface<std::string> {};

TEST_P(PlayerParamFixture, RunScript) {
    Poker::TableState table(Chips{1}, Chips{2});
    auto steps = load_file(GetParam());
    run_script(steps, table);
}

INSTANTIATE_TEST_SUITE_P(
    PlayerTest,
    PlayerParamFixture,
    ValuesIn(k_files)
);