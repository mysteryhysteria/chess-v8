#include <vector>
#include <array>
#include <bitset>
#include <string>
#include <assert.h>
#include "Move.h"
#include "Utils.h"

uint64_t merge_moves(std::vector<Move> moves) {
	uint64_t all_moves = 0;
	for (auto move : moves) {
		all_moves |= move.get_to();
	}
	return all_moves;
}

std::string p2an(uint64_t position)
{
	ASSERT_ONE_SQUARE(position);
	std::string an;
	int sq = 0;
	for (int i = 0; (position >>= 1) != 0; sq = ++i);
	auto file = sq % 8;
	auto rank = 7 - ((sq - file) / 8);

	return an.append(1, 'a' + file).append(1, '1' + rank);
}

