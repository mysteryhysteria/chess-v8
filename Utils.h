#pragma once
#include "assert.h"

#define ASSERT_ONE_SQUARE(square) (assert(std::bitset<64>((square).get_bitboard()).count() == 1))
