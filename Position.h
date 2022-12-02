#pragma once

#include <string>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include "stdint.h"
#include "Bitboards.h"
#include "Move.h"
#include "Types.h"
#include "Colors.h"
#include "Utils.h"
#include "Ray.h"

// TODO: change methods which have a "void" return type to instead return the Position object which they are called on if they mutate the state of the Position object.

const enum MoveOptions { PLACE = 1, CAPT = 2, SELF_CAPT = 4 };

const enum CastleSide {KINGSIDE = 0, QUEENSIDE = 1};

MoveOptions operator|(MoveOptions lhs, MoveOptions rhs);

struct perft_moves {
	int moves;
	int capts;
	int eps;
	int castles;
	int promos;
	int checks;
};

class Position {
private:
	// index these bitboard arrays with the enums defined above!
	std::array<Bitboard, 2> pieces_by_color; // array of bitboards, [0] for white pieces, [1] for black pieces
	std::array<Bitboard, 6> pieces_by_type; // array of bitboards, [0] pawns, [1] knights, [2] bishops, [3] rooks, [4] queens, [5] kings

	Square epsq; // bit board of where the en passant square is (if it exists) 

	uint16_t ply; // count of the current ply
	uint16_t ply_clock; // number of plys that have been played since the last capture or pawn move
	uint8_t flags;	// 4 bits for castling rights (0bxxx1-WK, 0bxx1x-WQ, 0bx1xx-BK, 0b1xxx-BQ)
					// 1 bit for in-check status (0b1-Check, 0b0-No Check)
					// 3 bonus bits!

	Bitboard checking_pieces; // bitboard of squares that are occupied by a piece which is checking the king
	Bitboard spying_pieces;	// bitboard of squares that are occupied by a piece which is "spying on" the king
	Bitboard pinned_pieces;	// bitboard of squares that are occupied by pinned pieces
	std::vector<Bitboard> check_vectors; // array of bitboards containing squares between checking pieces and the king
	std::vector<Bitboard> spy_vectors; // array of bitboards containing squares between spying pieces and the king


	std::vector<Move> move_gen_p(Square from);
	std::vector<Move> move_gen_k(Square from);
	std::vector<Move> move_gen_sliders(Square from, Types type);
	std::vector<Move> move_gen_generic(Square from, std::vector<int> directions, int max_distance = -1, MoveOptions move_opts = (MoveOptions::PLACE | MoveOptions::CAPT));

	// Implementing a simpler movegen algorithm in hopes that it will be more correct, and to aid in debugging. These methods are to support that effort.
	std::vector<Move> BASIC_pl_move_gen();
	std::vector<Move> BASIC_pl_std_move_gen(Square from);
	std::vector<Move> BASIC_pl_pawn_move_gen(Square from);
	std::vector<Move> BASIC_pl_castle_move_gen(Square from);

public:
	// Constructors
	Position(std::string fen) { parse_fen(fen); }
	Position() : Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {} // default standard starting position for chess, in FEN notation

	// Methods
	void parse_fen(std::string fen);
	//TODO std::string gen_fen();
	std::vector<Move> move_gen();
	static void disp_bitboard(Bitboard bb, std::string title, char piece_c, char empty_c);
	static void disp_bitboard(Bitboard bb, std::string title);
	static void disp_bitboard(Bitboard bb);
	Colors get_turn();
	Types get_type(Square sq);
	bool is_type(Square sq, Types type);
	bool is_open(Square sq);
	bool is_opponent(Square sq);
	bool is_friend(Square sq);
	bool is_castle_legal(CastleSide side);
	bool Q_castle_right();
	bool K_castle_right();
	void set_castle_right(Colors color, CastleSide side, bool set);
	bool is_in_check();
	bool square_covered(Square sq);
	bool attacked_by_piece(Square sq, Types piece_type);
	void king_threats();
	void set_in_check(bool set);
	void disp_bitboards();
	void disp_castling();
	void disp_epsq();
	void disp_plys();
	void disp(bool show_all);
	void disp() { disp(false); };
	Position& make_move(Move move);
	Position& undo();
	void perft(unsigned int depth, perft_moves& counts);
	void perft_core(unsigned int depth, perft_moves& counts);
	void BASIC_perft(unsigned int depth, perft_moves& counts);
	void BASIC_perft_core(unsigned int depth, perft_moves& counts);
	Bitboard get_occupied();
	void check_integrity();

	// Implementing a simpler movegen algorithm in hopes that it will be more correct, and to aid in debugging. These methods are to support that effort.
	std::vector<Move> BASIC_move_gen();
	void BASIC_king_threats(); // the basic one will only calculate if the king is in check.

};



