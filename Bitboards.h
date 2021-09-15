#pragma once

#include <bitset>
#include <map>
#include <array>
#include "stdint.h"
#include "Colors.h"
#include "Utils.h"
class Square;

class Bitboard {
private:
	uint64_t bitboard;

public:
	// Constructors
	explicit Bitboard(uint64_t bb);
	Bitboard(Square sq);
	explicit Bitboard();

	// Methods
	Bitboard& mark_square(Square& sq);
	Bitboard& clear_square(Square& sq);
	uint64_t get_bitboard();
	bool empty();
	bool contains(Square& sq);
	Square pop_occupied();

	// Operator Overloads
	Bitboard& operator|=(Square& sq);
	Bitboard& operator&=(Square& sq);
	Bitboard& operator|=(Bitboard bb);
	Bitboard& operator&=(Bitboard bb);
	Bitboard operator&(Square& sq);
	Bitboard operator|(Square& sq);
	Bitboard operator&(Bitboard& bb);
	Bitboard operator|(Bitboard& bb);
	
};

class Square {
private:
	uint64_t bitboard;

public:
	explicit Square(uint64_t sq);
	Square(Bitboard bb);
	explicit Square(int sq);
	explicit Square(unsigned int index);
	explicit Square(unsigned int rank, unsigned int file);
	explicit Square();

	// Operator overloads
	Bitboard operator&(const Square& rhs);
	Bitboard operator|(const Square& rhs);
	Bitboard operator&(Bitboard& rhs);
	Bitboard operator|(Bitboard& rhs);
	Bitboard operator~();
	Square& operator>>=(const unsigned int n);
	Square& operator<<=(const unsigned int n);
	Square& operator+=(const unsigned int n);
	Square& operator-=(const unsigned int n);
	Square operator>>(const unsigned int n);
	Square operator<<(const unsigned int n);
	bool operator!=(const Square& rhs);
	bool operator!=(const uint64_t& rhs);
	bool operator==(const Square& rhs);
	bool operator==(const uint64_t& rhs);


	// Methods
	bool empty();
	uint64_t get_bitboard();
	bool on_pawn_start_rank(Colors turn);
	bool on_promote_rank(Colors turn);
	bool on_nth_rank(unsigned int n);
	bool on_nth_file(unsigned int n);
	unsigned int convert_to_index();
	std::string p2an();
	Square& next(unsigned int skip = 1);
	Square& previous(unsigned int skip = 1);
	Square& first();
};

uint64_t operator&=(uint64_t lhs, Square rhs);
uint64_t operator|=(uint64_t lhs, Square rhs);


