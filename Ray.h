#pragma once
#include "Bitboards.h"
#include <unordered_map>

// TODO: Reimplement the Ray to use iterators (more idiomatic).

enum class Directions {
	N	=  -8,
	NNE =  -15,
	NE	=  -7,
	ENE =  -6,
	E	=  1,
	ESE =  10,
	SE	=  9,
	SSE	=  17,
	S	=  8,
	SSW	=  15,
	SW	=  7,
	WSW	=  6,
	W	= -1,
	WNW	= -10,
	NW	= -9,
	NNW	= -17,
	NN	= -16,
	EE	=  2,
	SS	=  16,
	WW	= -2,
	NO_DIRECTION = 0,
	BLACK_PAWN_CAPTURE_WEST = SW,
	BLACK_PAWN_CAPTURE_EAST = SE,
	BLACK_PAWN_PUSH = S,
	BLACK_PAWN_DOUBLE_PUSH = SS,
	WHITE_PAWN_CAPTURE_EAST = NE,
	WHITE_PAWN_CAPTURE_WEST = NW,
	WHITE_PAWN_PUSH = N,
	WHITE_PAWN_DOUBLE_PUSH = NN,
	QUEENSIDE_CASTLE = WW,
	KINGSIDE_CASTLE = EE
};

bool operator==(Directions lhs, int rhs);
bool operator==(int lhs,		Directions rhs);
bool operator==(Directions lhs, Directions rhs);

const std::unordered_map<Directions, Bitboard> move_masks = { // map of move directions to bitmasks for the move generation
		{Directions::NNW, Bitboard(~0x010101010101ffff)},
		{Directions::NNE, Bitboard(~0x808080808080ffff)},
		{Directions::WNW, Bitboard(~0x03030303030303ff)},
		{Directions::NW,  Bitboard(~0x01010101010101ff)},
		{Directions::N,   Bitboard(~0x00000000000000ff)},
		{Directions::NE,  Bitboard(~0x80808080808080ff)},
		{Directions::ENE, Bitboard(~0xc0c0c0c0c0c0c0ff)},
		{Directions::WW,  Bitboard(~0x0303030303030303)},
		{Directions::W,   Bitboard(~0x0101010101010101)},
		{Directions::E,   Bitboard(~0x8080808080808080)},
		{Directions::EE,  Bitboard(~0xc0c0c0c0c0c0c0c0)},
		{Directions::WSW, Bitboard(~0xff03030303030303)},
		{Directions::SW,  Bitboard(~0xff01010101010101)},
		{Directions::S,   Bitboard(~0xff00000000000000)},
		{Directions::SE,  Bitboard(~0xff80808080808080)},
		{Directions::ESE, Bitboard(~0xffc0c0c0c0c0c0c0)},
		{Directions::SSW, Bitboard(~0xffff010101010101)},
		{Directions::SSE, Bitboard(~0xffff808080808080)}
};

const int NO_MAX = -1;

class Ray {
private:
	Square current;
	Square base;
	int direction;
	int distance;
	bool maxed;
	Square get_next();
	Square get_prev();
	void update_maxed();

public:
	Ray(Square base, Directions direction, int max_distance = -1) : current(base), base(base), direction(static_cast<int>(direction)), distance(max_distance) { update_maxed(); };
	Ray(Square base) : Ray(base, Directions::NO_DIRECTION, 0) {};
	Ray() : Ray(Square(), Directions::NO_DIRECTION, 0) {};

	Ray& operator++();
	Ray& operator--();

	Square get_base();
	Square get_current();
	Ray& reset(Directions dir = Directions::NO_DIRECTION, int max_distance = -1);
	bool end();
};

