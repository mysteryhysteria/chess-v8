#pragma once
#include <vector>
#include <assert.h>
#include "Move.h"

#define ASSERT_ONE_SQUARE(bb) (assert(std::bitset<64>(bb).count() == 1))

uint64_t merge_moves(std::vector<Move> moves);
bool on_nth_rank(uint64_t square, unsigned int n);
bool on_nth_file(uint64_t square, unsigned int n);
std::string p2an(uint64_t position);