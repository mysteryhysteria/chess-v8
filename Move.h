#pragma once
#include <map>
#include <vector>
#include "Colors.h"
#include "Types.h"
#include "SpecialMoves.h"

class Move {
private:
	uint64_t from, to, special; // special will be used to store information about the following: the captured e.p. pawn if this is an e.p. move; the castling rook if this is a castle move
	Colors color;
	Types type, capt_type, promote_type;
	SpecialMoves move_type;

public:
	// for general moves
	Move(uint64_t from, uint64_t to, uint64_t special, Colors color, Types type, Types capt_type, Types promote_type, SpecialMoves move_type) {
		this->from = from;
		this->to = to;
		this->special = special;
		this->color = color;
		this->type = type;
		this->capt_type = capt_type;
		this->promote_type = promote_type;
		this->move_type = move_type;
	}
	// for standard capture moves
	Move(uint64_t from, uint64_t to, Colors color, Types type, Types capt_type) : Move(from, to, 0ULL, color, type, capt_type, Types::NONE, SpecialMoves::STD) {}
	// for standard positional moves
	Move(uint64_t from, uint64_t to, Colors color, Types type) : Move(from, to, color, type, Types::NONE) {}

	Move() : Move(0ULL, 0ULL, Colors::WHITE, Types::NONE) {}
};