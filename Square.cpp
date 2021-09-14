#include "Square.h"

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

bool Square::on_nth_rank(unsigned int n) {
	return bool(this->bitboard & rank_masks[n]);
}

bool Square::on_nth_file(unsigned int n) {
	return bool(this->bitboard & file_masks[n]);
}

bool Square::on_pawn_start_rank(Colors turn) {
	unsigned int n;
	!(turn) ? n = 1 : n = 6;
	return this->on_nth_rank(n);
}

bool Square::on_promote_rank(Colors turn) {
	unsigned int n;
	!(turn) ? n = 7 : n = 0;
	return this->on_nth_rank(n);
}

unsigned int Square::convert_to_index() {
	unsigned int maxbit = 0;
	while ((*this >> (maxbit + 1)) != 0) maxbit++;
	return maxbit;
}

std::string Square::p2an() {
	std::string an;
	int sq = this->convert_to_index();
	auto file = sq % 8;
	auto rank = 7 - ((sq - file) / 8);

	return an.append(1, 'a' + file).append(1, '1' + rank);
}

uint64_t operator&=(uint64_t lhs, Square rhs) { return lhs &= (rhs.get_bitboard()); };
uint64_t operator|=(uint64_t lhs, Square rhs) { return lhs |= (rhs.get_bitboard()); };