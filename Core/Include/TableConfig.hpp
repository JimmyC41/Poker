#pragma once

#include <concepts>
#include "Chips.hpp"

namespace Poker {

struct DefaultBlinds {
    static constexpr Chips small_blind{1};
    static constexpr Chips big_blind{2};
};

template<typename T>
concept ConfigConcept = requires {
    { T::num_players } -> std::convertible_to<std::size_t>;
    
    typename T::Blinds;
    { T::Blinds::small_blind } -> std::convertible_to<Chips>;
    { T::Blinds::big_blind } -> std::convertible_to<Chips>;
};

struct HeadsUpConfig {
    static constexpr std::size_t num_players = 2;
    using Blinds = Poker::DefaultBlinds;
};

struct EightMaxConfig {
    static constexpr std::size_t num_players = 8;
    using Blinds = Poker::DefaultBlinds;
};

} // namespace Poker