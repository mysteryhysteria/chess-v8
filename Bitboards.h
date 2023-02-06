#pragma once

#include <bitset>
#include <map>
#include <array>
#include <optional>
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
	explicit Square(unsigned int rank, unsigned int file);
	explicit Square();

	// Operator overloads
	Bitboard operator&(const Square rhs);
	Bitboard operator|(const Square rhs);
	Bitboard operator&(Bitboard rhs);
	Bitboard operator|(Bitboard rhs);
	Bitboard operator~();
	Square& operator>>=(const unsigned int n);
	Square& operator<<=(const unsigned int n);
	Square& operator+=(const unsigned int n);
	Square& operator-=(const unsigned int n);
	Square operator>>(const unsigned int n);
	Square operator<<(const unsigned int n);
	bool operator!=(const Square rhs);
	bool operator!=(const uint64_t rhs);
	bool operator==(const Square rhs);
	bool operator==(const uint64_t rhs);


	// Methods
	bool is_empty();
	uint64_t get_u64();
	bool on_pawn_start_rank(Colors turn);
	bool on_promote_rank(Colors turn);
	bool on_nth_rank(unsigned int n);
	bool on_nth_file(unsigned int n);
	unsigned int convert_to_index();
	std::string p2an();
	Square& next(unsigned int skip = 1);
	Square& previous(unsigned int skip = 1);
	Square& first();
	int popcount();
	int direction_from(Square from);
};

uint64_t operator&=(uint64_t lhs, Square rhs);
uint64_t operator|=(uint64_t lhs, Square rhs);


