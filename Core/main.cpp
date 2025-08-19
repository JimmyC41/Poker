#include <iostream>
#include "Handler.hpp"

int main() {
    using namespace Poker;
    TableState ts(Chips{1}, Chips{2});
    apply(ts, AddPlayer{1, Chips{10}});
}