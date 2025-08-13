#pragma once

#include <cstdint>

namespace Poker {

enum class Street : uint8_t {
    PreDeal = 0,
    PreFlop,
    Flop,
    Turn,
    River,
    Showdown,
    HandOver
};

}