#include <iostream>
#include "Deck.hpp"
#include "TableState.hpp"

int main() {
    Poker::Deck<> deck;
    std::cout << deck.to_string();
    return 0;
}