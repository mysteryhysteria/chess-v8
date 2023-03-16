#pragma once

#include <vector>
#include "Bitboards.h"
#include "Colors.h"
#include "Types.h"
#include "SpecialMoves.h"

const uint64_t FOURTH_RANK = 0x000000ff00000000;
const uint64_t FIFTH_RANK  = 0x00000000ff000000;
const uint64_t WHITE_ROOKS_STARTING_POS = 0x8100000000000000;
const uint64_t BLACK_ROOKS_STARTING_POS = 0x0000000000000081;

// TODO: restructure the move class to be more minimal by removing the "type" and "capt_type" from the class. These are redundant and can be computed from the board state. 
// this may be helpful in reducing the number of possible errors that can be introduced when creating moves.
class Move {
private:
	Square from, to, special; // special will be used to store information about the following: the captured e.p. pawn if this is an e.p. move; the castling rook if this is a castle move
	Colors color;
	Types type, capt_type, promote_type;
	SpecialMoves move_type;

public:
	// Constructors
	// for general moves
	Move(Square from, Square to, Square special, Colors color, Types type, Types capt_type, Types promote_type, SpecialMoves move_type) {
		this->from = from;
		this->to = to;
		this->special = special;
		this->color = color;
		this->type = type;
		this->capt_type = capt_type;
		this->promote_type = promote_type;
		this->move_type = move_type;

		// input checking - these checks are not fully comprehensive as the move object doesn't have any information about the board.
		// In order to make fully comprehensive checks, the current position of the board needs to be taken into consideration.

		// from can never be empty
		assert(from.popcount() == 1);

		// to can never be empty
		assert(to.popcount() == 1);

		// special must not be empty if the move type is castle
		assert((move_type == SpecialMoves::CASTLE)	   ? (special.popcount() == 1) : true);
		// special must not be empty if the move type is en_passant
		assert((move_type == SpecialMoves::EN_PASSANT) ? (special.popcount() == 1) : true);
		// special must be on the 6th rank if move type is en passant and the color is white
		assert((move_type == SpecialMoves::EN_PASSANT && color == Colors::WHITE) ? (special.get_u64() & FIFTH_RANK != 0ULL) : true);
		// special must be on the 3rd rank if move type is en passant and the color is black
		assert((move_type == SpecialMoves::EN_PASSANT && color == Colors::BLACK) ? (special.get_u64() & FOURTH_RANK != 0ULL) : true);
		// special must be either a1 or h1 if move type is castle and the color is white
		assert((move_type == SpecialMoves::CASTLE && color == Colors::WHITE) ? (special.get_u64() & WHITE_ROOKS_STARTING_POS != 0ULL) : true);
		// special must be either a8 or h8 if move type is castle and the color is black
		assert((move_type == SpecialMoves::CASTLE && color == Colors::BLACK) ? (special.get_u64() & BLACK_ROOKS_STARTING_POS != 0ULL) : true);
		// special must be empty if the move type is std
		assert((move_type == SpecialMoves::STD)		  ? (special.popcount() == 0) : true);
		// special must be empty if the move type is promotion
		assert((move_type == SpecialMoves::PROMOTION) ? (special.popcount() == 0) : true);
		
		// color can be any valid color.

		// type cannot be none
		assert(type != Types::NONE);
		// type must be pawn if move type is promotion
		assert((move_type == SpecialMoves::PROMOTION) ? (type == Types::PAWN) : true);
		// type must be king if move type is castle
		assert((move_type == SpecialMoves::CASTLE) ? (type == Types::KING) : true);
		// type must be pawn if move type is en passant
		assert((move_type == SpecialMoves::EN_PASSANT) ? (type == Types::PAWN) : true);

		// capt_type cannot be a king
		assert(capt_type != Types::KING);
		// capt_type must be a pawn if move type is en passant
		assert((move_type == SpecialMoves::EN_PASSANT) ? (capt_type == Types::PAWN) : true);

		// promote type cannot be a king
		assert(promote_type != Types::KING);
		// promote type cannot be a pawn
		assert(promote_type != Types::PAWN);
		// promote type must be none if move type is not promotion
		assert((move_type != SpecialMoves::PROMOTION) ? (promote_type == Types::NONE) : true);
		// promote type must not be none if move type is promotion
		assert((move_type == SpecialMoves::PROMOTION) ? (promote_type != Types::NONE) : true);
	}
	// for standard capture moves
	Move(Square from, Square to, Colors color, Types type, Types capt_type) : Move(from, to, Square(), color, type, capt_type, Types::NONE, SpecialMoves::STD) {}

	// for standard positional moves
	Move(Square from, Square to, Colors color, Types type) : Move(from, to, color, type, Types::NONE) {}

	Move() : Move(Square(), Square(), Colors::WHITE, Types::NONE) {}

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

Bitboard merge_moves(std::vector<Move> moves);
std::ostream& operator<<(std::ostream& out, Move move);