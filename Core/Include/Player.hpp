#pragma once

#include <string>
#include <memory>
#include "Chips.hpp"

namespace Poker {

using PlayerId = int;

class Player {
public:
    explicit Player(PlayerId id) : m_id(id) {}
    PlayerId id() const { return m_id; }
private:
    PlayerId m_id;
};

using PlayerPtr = std::unique_ptr<Player>;

}