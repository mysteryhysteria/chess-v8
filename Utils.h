#pragma once
#include "assert.h"

#define ASSERT_ONE_SQUARE(square) (assert(std::bitset<64>((square).get_u64()).count() == 1))

#ifdef BASIC
#define RUN_PERFT(x, y)	BASIC_perft(x, y)
#else
#define RUN_PERFT(x, y) perft(x, y)
#endif