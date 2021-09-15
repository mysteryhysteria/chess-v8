#pragma once

#include <string>
#include <vector>
#include <array>
#include <map>
#include "stdint.h"
#include "Bitboards.h"
#include "Move.h"
#include "Types.h"
#include "Colors.h"
#include "Utils.h"
#include "Ray.h"

const enum MoveOptions { PLACE = 1, CAPT = 2, SELF_CAPT = 4 };

MoveOptions operator|(MoveOptions lhs, MoveOptions rhs);

class Position {
private:
	// index these bitboard arrays with the enums defined above!
	std::array<Bitboard, 2> pieces_by_color; // array of bitboards, [0] for white pieces, [1] for black pieces
	std::array<Bitboard, 6> pieces_by_type; // array of bitboards, [0] pawns, [1] knights, [2] bishops, [3] rooks, [4] queens, [5] kings

	Square epsq; // bit board of where the en passant square is (if it exists) 

	uint16_t ply; // count of the current ply
	uint16_t ply_clock; // number of plys that have been played since the last capture or pawn move
	uint8_t flags;	// 4 bits for castling rights (0bxxx1-WK, 0bxx1xb-WQ, 0bx1xxb-BK, 0b1xxxb-BQ)
					// 1 bit for in-check status (0b1-Check, 0b0-No Check)
					// 3 bonus bits!

	std::array<Bitboard, 12> checking_pieces; // array containing squares that are occupied by a piece which is checking the king
	std::array<Bitboard, 12> pinned_pieces;	// array containing squares that are occupied by pinned pieces


	std::vector<Move> move_gen_p(Square from);
	std::vector<Move> move_gen_k(Square from);
	std::vector<Move> move_gen_sliders(Square from, Types type);
	std::vector<Move> move_gen_generic(Square from, std::vector<int> directions, unsigned int max_distance = -1, MoveOptions move_opts = (MoveOptions::PLACE | MoveOptions::CAPT));

public:
	// Testing - do not leave here!

	// Constructors
	Position(std::string fen) { parse_fen(fen); }
	Position() : Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {} // default standard starting position for chess, in FEN notation

	// Methods
	void parse_fen(std::string fen);
	std::vector<Move> move_gen();
	static void disp_bitboard(Bitboard bb, std::string title, char piece_c, char empty_c);
	static void disp_bitboard(Bitboard bb, std::string title);
	static void disp_bitboard(Bitboard bb);
	Colors get_turn();
	Types get_type(Square sq);
	bool is_type(Square sq, Types type);
	bool is_opponent(Square sq);
	bool is_friend(Square sq);
	bool Q_castle_right();
	bool K_castle_right();
	bool in_check();
	bool square_covered(Square sq);
	bool attacked_by_piece(Square sq, Types piece_type);
	void set_in_check();
	void disp_bitboards();
	void disp_castling();
	void disp_epsq();
	void disp_plys();
	void disp();

};



