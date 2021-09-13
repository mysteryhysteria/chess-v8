#include "Square.h"

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

bool Square::attacked_by_piece(Types piece_type, Position position) {
	auto turn = position.get_turn();
	uint64_t mask;
	Square to = *this; // initialize the to square to be the current square
	unsigned int gen_dist,
		max_distance = -1;

	if (piece_type == PAWN || piece_type == KNIGHT || piece_type == KING) { max_distance = 1; }

	if (piece_type == PAWN) { auto directions = pawn_capt_directions[turn]; }
	else { auto directions = move_directions[piece_type]; }

	for (auto dir : move_directions[piece_type]) {
		gen_dist = 0;
		mask = move_masks[dir];
		while (((to & mask) != 0) && (gen_dist++ != max_distance)) {
			(dir < 0) ? (to >>= abs(dir)) : (to <<= dir);
			if (position.is_opponent(to)) { // found an opponent's piece
				if (position.is_type(to, piece_type)) {
					return true;
				}
			}
			else if (position.is_friend(to)) { // found a friendly piece, which blocks enemy pieces!
				break;
			}
			// otherwise continue the search
		}
		to = *this;
	}
	return false;
}

bool Square::square_covered(Colors turn, Position position) {
	for (int i = 0; i < 6; ++i) {
		if (attacked_by_piece(static_cast<Types>(i), position)) {
			return true;
		}
	}
	return false;
}