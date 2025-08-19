#pragma once

#include "Command.hpp"
#include "TableState.hpp"

namespace Poker {

// Cmd reducer, std::visit 
void apply(TableState& s, const Command& c);

// Overload struct which exposes operator() overloads for inherited types
template<class... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};

// CTAD
template<class... Ts>
Overload(Ts...) -> Overload<Ts...>;

} // namespace Poker