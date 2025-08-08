#include <iostream>
#include <Deck.hpp>

int main() {
    Poker::Deck<> deck;
    std::cout << deck.to_string();
    return 0;
}