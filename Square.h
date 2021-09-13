#pragma once
#include <bitset>
#include "stdint.h"
#include "Utils.h"
#include "Position.h"

class Square {
private:
	uint64_t bitboard;

	bool on_nth_rank(unsigned int n);
	bool on_nth_file(unsigned int n);

public:
	Square(uint64_t square) {
		bitboard = square;
		ASSERT_ONE_SQUARE(*this);
	}
	explicit Square(unsigned int n) : Square(uint64_t(n)) {};
	explicit Square(int n) : Square(uint64_t(n)) {};

	uint64_t operator&(const Square& rhs) { return this->bitboard & rhs.bitboard; };
	uint64_t operator|(const Square& rhs) { return this->bitboard | rhs.bitboard; };
	Square& operator>>=(const int n) { this->bitboard >>= n; return *this; };
	Square& operator<<=(const int n) { this->bitboard <<= n; return *this; };
	Square operator>>(const int n) { return Square(this->bitboard >> n); };
	Square operator<<(const int n) { return Square(this->bitboard << n); };
	bool operator!=(const Square& rhs) { return bool(this->bitboard != rhs.bitboard); };
	bool operator==(const Square& rhs) { return bool(this->bitboard == rhs.bitboard); };
	bool empty() { return *this == 0; };

	uint64_t get_bitboard() { return this->bitboard; };
	bool square_covered(Colors turn, Position position);
	bool on_pawn_start_rank(Colors turn);
	bool on_promote_rank(Colors turn);
	bool attacked_by_piece(Types piece_type, Position position);
	unsigned int convert_to_index();
};

