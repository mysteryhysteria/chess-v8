#pragma once
#include <map>
#include <vector>
#include "Colors.h"
#include "Types.h"
#include "SpecialMoves.h"

class Move {
private:
	Square from, to, special; // special will be used to store information about the following: the captured e.p. pawn if this is an e.p. move; the castling rook if this is a castle move
	Colors color;
	Types type, capt_type, promote_type;
	SpecialMoves move_type;

public:
	// Constructors
	// for general moves
	Move(Square from, Square to, Square special, Colors color, Types type, Types capt_type, Types promote_type, SpecialMoves move_type) : 
		from(from), to(to), special(special), color(color), type(type), capt_type(capt_type), promote_type(promote_type), move_type(move_type) {}
	// for standard capture moves
	Move(Square from, Square to, Colors color, Types type, Types capt_type) : Move(from, to, Square(0ULL), color, type, capt_type, Types::NONE, SpecialMoves::STD) {}
	// for standard positional moves
	Move(Square from, Square to, Colors color, Types type) : Move(from, to, color, type, Types::NONE) {}

	Move() : Move(0ULL, 0ULL, Colors::WHITE, Types::NONE) {}

	// general purpose
	Move& set_special(Square special) { this->special = special; return *this; }
	Move& set_move_type(SpecialMoves move_type) { this->move_type = move_type; return *this; }
	Move& set_promote_type(Types promote_type) { this->promote_type = promote_type; return *this; }
	Square get_from() { return this->from; }
	Square get_to() { return this->to; }
	Square get_special() { return this->special; }
	Colors get_color() { return this->color; }
	Types get_type() { return this->type; }
	Types get_capt_type() { return this->capt_type; }
	Types get_promote_type() { return this->promote_type; }
	SpecialMoves get_move_type() { return this->move_type; }

};

std::ostream& operator<<(std::ostream& out, Move move);