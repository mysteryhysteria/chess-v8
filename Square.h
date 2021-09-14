#pragma once

#include <bitset>
#include <map>
#include <array>
#include "stdint.h"
#include "Colors.h"
#include "Utils.h"

class Square {
private:
	uint64_t bitboard;

public:
	Square(uint64_t square) {
		bitboard = square;
		ASSERT_ONE_SQUARE(*this);
	}
	Square(unsigned int n) : Square(uint64_t(n)) {};
	Square(int n) : Square(uint64_t(n)) {};
	Square() : Square(0ULL) {};

	// Operator overloads
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
	bool on_pawn_start_rank(Colors turn);
	bool on_promote_rank(Colors turn);
	bool on_nth_rank(unsigned int n);
	bool on_nth_file(unsigned int n);
	unsigned int convert_to_index();
	std::string p2an();
};

uint64_t operator&=(uint64_t lhs, Square rhs);
uint64_t operator|=(uint64_t lhs, Square rhs);


