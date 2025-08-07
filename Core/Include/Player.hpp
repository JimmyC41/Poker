#pragma once

#include <string>
#include "Chips.hpp"

namespace Poker {

class Player {
    explicit Player(int id) : m_id(id) {}
private:
    int m_id;
};

}