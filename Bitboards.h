#pragma once

#include <bitset>
#include <array>
#include <optional>
#include <unordered_map>
#include "stdint.h"
#include "Colors.h"
#include "Utils.h"

inline std::optional<int> an2idx(std::string an) {
	auto idx = std::optional<int>(std::nullopt);
	if (an.size() != 2) { return idx; };
	an[0] = std::tolower(an[0]); // allows the function to accept uppercase letters for the file.
	if (an[0] < 'a' || an[0] > 'h') { return idx; };
	if (an[1] < '1' || an[1] > '8') { return idx; };

	// get rank number from 0-8
	auto rank = 8 - (an[1] - '0');
	// get file number from 0-8
	auto file = an[0] - 'a';
	idx = rank * 8 + file;
	return idx;
}

inline std::optional<std::string> idx2an(int i) {
	if (i < 0 || i >= 64) { return std::optional<std::string>(std::nullopt); };
	int file = i % 8;
	int rank = 7 - ((i - file) / 8);

	std::string an = std::string();
	an.push_back('a' + file);
	an.push_back('1' + rank);

	return std::make_optional(an);
}

class Square;
enum class Directions;

class Bitboard {
private:
	uint64_t bitboard;

public:
	// Constructors
	explicit Bitboard(uint64_t bb);
	Bitboard(Square sq);
	explicit Bitboard();

	// Methods
	Bitboard& mark_square(Square sq);
	Bitboard& clear_square(Square sq);
	Bitboard& clear();
	Bitboard& mirror_v();
	uint64_t get_u64();
	bool is_empty();
	bool contains(Square sq);
	Square pop_occupied();
	int popcount();

	// Operator Overloads
	Bitboard& operator|=(Square sq);
	Bitboard& operator&=(Square sq);
	Bitboard& operator|=(Bitboard bb);
	Bitboard& operator&=(Bitboard bb);
	Bitboard operator&(Square sq);
	Bitboard operator|(Square sq);
	Bitboard operator&(Bitboard bb);
	Bitboard operator|(Bitboard bb);
	Bitboard operator~();
	
};
class Square {
private:
	uint64_t bitboard;

public:
	explicit Square(uint64_t sq);
	Square(Bitboard bb);
	explicit Square(int sq);
	explicit Square(unsigned int index);

	// TODO: change implementation so that rank and file arent 0 indexed. Or just rely on the string based constructor and delete this or make it private
	explicit Square(unsigned int rank, unsigned int file); 
	
	// TODO: add constructor to build squares from strings
	
	explicit Square();

	// Operator overloads
	Bitboard operator&(const Square rhs);
	Bitboard operator|(const Square rhs);
	Bitboard operator&(Bitboard rhs);
	Bitboard operator|(Bitboard rhs);
	Bitboard operator~();
	Square operator>>(const unsigned int n);
	Square operator<<(const unsigned int n);
	Square& operator>>=(const unsigned int n);
	Square& operator<<=(const unsigned int n);
	Square operator+(const int n);
	Square operator-(const int n);
	Square& operator+=(const int n);
	Square& operator-=(const int n);
	Square operator+(const Directions dir);
	Square operator-(const Directions dir);
	Square& operator+=(const Directions dir);
	Square& operator-=(const Directions dir);
	bool operator!=(const Square rhs);
	bool operator!=(const uint64_t rhs);
	bool operator==(const Square rhs);
	bool operator==(const uint64_t rhs);


	// Methods
	bool is_empty();
	uint64_t get_u64() const;
	bool on_pawn_start_rank(Colors turn);
	bool on_promote_rank(Colors turn);
	bool on_nth_rank(unsigned int n);
	bool on_nth_file(unsigned int n);
	unsigned int convert_to_index();
	std::string p2an();
	std::optional<Square> next(unsigned int skip = 1);
	std::optional<Square> previous(unsigned int skip = 1);
	Square& first();
	int popcount();
	int direction_from(Square from);
	bool is_square(std::string an);
};

enum class Directions {
	N = -8,
	NNE = -15,
	NE = -7,
	ENE = -6,
	E = 1,
	ESE = 10,
	SE = 9,
	SSE = 17,
	S = 8,
	SSW = 15,
	SW = 7,
	WSW = 6,
	W = -1,
	WNW = -10,
	NW = -9,
	NNW = -17,
	NN = -16,
	EE = 2,
	SS = 16,
	WW = -2,
	EEE = 3,
	NO_DIRECTION = 0,
	BLACK_PAWN_CAPTURE_WEST = SW,
	BLACK_PAWN_CAPTURE_EAST = SE,
	BLACK_PAWN_PUSH = S,
	BLACK_PAWN_DOUBLE_PUSH = SS,
	WHITE_PAWN_CAPTURE_EAST = NE,
	WHITE_PAWN_CAPTURE_WEST = NW,
	WHITE_PAWN_PUSH = N,
	WHITE_PAWN_DOUBLE_PUSH = NN,
	KING_QUEENSIDE_CASTLE = WW,
	KING_KINGSIDE_CASTLE = EE,
	ROOK_QUEENSIDE_CASTLE = EEE,
	ROOK_KINGSIDE_CASTLE = WW
};

bool operator==(Directions lhs, int rhs);
bool operator==(int lhs, Directions rhs);
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

uint64_t operator&=(uint64_t lhs, Square rhs);
uint64_t operator|=(uint64_t lhs, Square rhs);


