#pragma once

#include <string>
#include <vector>
#include <array>
#include "stdint.h"
#include "move.h"

enum Colors { WHITE = 0, BLACK = 1 };
enum Types { PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5 };


class Position {
private:
	// index these bitboard arrays with the enums defined above!
	std::array<uint64_t, 2> pieces_by_color; // array of bitboards, [0] for white pieces, [1] for black pieces
	std::array<uint64_t, 6> pieces_by_type; // array of bitboards, [0] pawns, [1] knights, [2] bishops, [3] rooks, [4] queens, [5] kings

	uint64_t epsq; // bit board of where the en passant square is (if it exists) 

	uint16_t ply; // count of the current ply
	uint16_t ply_clock; // number of plys that have been played since the last capture or pawn move
	uint8_t flags;	// 4 bits for castling rights (xxx1b-WK, xx1xb-WQ, x1xxb-BK, 1xxxb-BQ)
					// 2 bits for in-check status (x1b-W, 1xb-B)
					// 2 bonus bits!

public:
	// Constructors
	Position(std::string fen) { parse_fen(fen); }
	Position() : Position( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" ) {} // default standard starting position for chess, in FEN notation

	// Methods
	void parse_fen(std::string fen);
	std::vector<Move> move_gen(unsigned int depth);
	static void display_bitboard(uint64_t bitboard);
	static void display_bitboard(uint64_t bitboard, std::string title);
	void display_bitboards();
	void console_display();

};
