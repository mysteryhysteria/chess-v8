#pragma once

#include <string>
#include <vector>
#include <array>
#include "stdint.h"
#include "Move.h"
#include "Types.h"
#include "Colors.h"

const enum MoveOptions { PLACE = 1, CAPT = 2, SELF_CAPT = 4 };

MoveOptions operator|(MoveOptions lhs, MoveOptions rhs);

class Position {
private:
	// index these bitboard arrays with the enums defined above!
	std::array<uint64_t, 2> pieces_by_color; // array of bitboards, [0] for white pieces, [1] for black pieces
	std::array<uint64_t, 6> pieces_by_type; // array of bitboards, [0] pawns, [1] knights, [2] bishops, [3] rooks, [4] queens, [5] kings

	uint64_t epsq; // bit board of where the en passant square is (if it exists) 

	uint16_t ply; // count of the current ply
	uint16_t ply_clock; // number of plys that have been played since the last capture or pawn move
	uint8_t flags;	// 4 bits for castling rights (0bxxx1-WK, 0bxx1xb-WQ, 0bx1xxb-BK, 0b1xxxb-BQ)
					// 1 bit for in-check status (0b1-Check, 0b0-No Check)
					// 3 bonus bits!

	std::vector<Move> move_gen_p(uint64_t from);
	std::vector<Move> move_gen_k(uint64_t from);
	std::vector<Move> move_gen_sliders(uint64_t from, Types type);
	std::vector<Move> move_gen_generic(uint64_t from, std::vector<int> directions, unsigned int max_distance = -1, MoveOptions move_opts = (MoveOptions::PLACE | MoveOptions::CAPT));
	bool attacked_by_piece(Types piece_type, uint64_t from);
	bool square_covered(uint64_t from);
	void set_in_check();

public:
	// Testing - do not leave here!

	// Constructors
	Position(std::string fen) { parse_fen(fen); }
	Position() : Position( "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" ) {} // default standard starting position for chess, in FEN notation

	// Methods
	void parse_fen(std::string fen);
	std::vector<Move> move_gen();
	static void disp_bitboard(uint64_t bitboard, std::string title, char piece_c, char empty_c);
	static void disp_bitboard(uint64_t bitboard, std::string title);
	static void disp_bitboard(uint64_t bitboard);
	Colors get_turn();
	Types get_type(uint64_t square);
	bool on_pawn_start_rank(uint64_t square);
	bool on_promote_rank(uint64_t square);
	bool Q_castle_right();
	bool K_castle_right();
	bool in_check();
	void disp_bitboards();
	void disp_castling();
	void disp_epsq();
	void disp_plys();
	void disp();

};
