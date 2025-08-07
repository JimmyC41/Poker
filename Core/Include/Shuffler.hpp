#pragma once

#include <concepts>
#include <ranges>
#include <random>
#include <algorithm>

namespace Poker {
/**
 * Note:
 * A seeder creates a random generation instance.
 * A random generator produces a sequence of pseudo-random bits.
 * A policy implements the shuffling using the random generator.
 */

// Card is a mutable, random access range
template<typename Card>
concept ContainerConcept =
    std::ranges::random_access_range<Card> &&
    std::ranges::output_range<Card, std::ranges::range_value_t<Card>>;

// Produces uniformly distributed pseudo-random unsigned integers
template<typename RNG>
concept RNGConcept = std::uniform_random_bit_generator<RNG>;

// Seeder can be default-ctored and produces the correct generator
template<typename Seeder, typename RNG>
concept SeederConcept = 
    std::default_initializable<Seeder> &&
    requires(Seeder s) {
        {s()} -> std::convertible_to<RNG>;
};

// Policy exposes a method to shuffle the container provided a generator
template<typename Container, typename RNG, typename Policy>
concept PolicyConcept = requires(Container& c, RNG& r, Policy& p) {
    p(c, r); 
};

// Combined Concept for Deck
template<typename Container, typename RNG, typename Seeder, typename Policy>
concept DeckConcept =
    ContainerConcept<Container> &&
    RNGConcept<RNG> &&
    SeederConcept<Seeder, RNG> &&
    PolicyConcept<Container, RNG, Policy>;

// Default Seeder for Deck
template <typename RNG>
struct RandomSeeder {
    RNG operator()() const {
        return RNG{std::random_device{}()};
    }
};

// Default Policy for Deck
struct FisherYates {
    // std::ranges_random_access_range: any container which provides begin(), end() and random access
    // std::uniform_random_bit_generator: any uniform random bit generator
    
    template<ContainerConcept Container, RNGConcept RNG>
    void operator()(Container& c, RNG& r) const {
        std::shuffle(std::begin(c), std::end(c), r); // Fisher-Yates shuffles
    }
};

}