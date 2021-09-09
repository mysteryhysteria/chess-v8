#include <vector>
#include <array>
#include <bitset>
#include <string>
#include <assert.h>
#include "Move.h"
#include "Utils.h"

std::array<uint64_t, 8> rank_masks = {
	0xff00000000000000,
	0x00ff000000000000,
	0x0000ff0000000000,
	0x000000ff00000000,
	0x00000000ff000000,
	0x0000000000ff0000,
	0x000000000000ff00,
	0x00000000000000ff
};

std::array<uint64_t, 8> file_masks = {
	0x0101010101010101,
	0x0202020202020202,
	0x0404040404040404,
	0x0808080808080808,
	0x1010101010101010,
	0x2020202020202020,
	0x4040404040404040,
	0x8080808080808080
};

uint64_t merge_moves(std::vector<Move> moves) {
	uint64_t all_moves = 0;
	for (auto move : moves) {
		all_moves |= move.get_to();
	}
	return all_moves;
}

bool on_nth_rank(uint64_t square, unsigned int n) {
	ASSERT_ONE_SQUARE(square);
	return bool (square & rank_masks[n]);
}

bool on_nth_file(uint64_t square, unsigned int n) {
	ASSERT_ONE_SQUARE(square);
	return bool(square & file_masks[n]);
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

