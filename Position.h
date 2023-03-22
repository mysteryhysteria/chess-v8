#pragma once

#include <string>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "stdint.h"
#include "Bitboards.h"
#include "Move.h"
#include "Types.h"
#include "Colors.h"
#include "Utils.h"
#include "Ray.h"

// TODO: change methods which have a "void" return type to instead return the Position object which they are called on if they mutate the state of the Position object.

const enum class MoveOptions { PLACE = 1, CAPT = 2, SELF_CAPT = 4 };

const enum class CastleSide {KINGSIDE = 0, QUEENSIDE = 1};

MoveOptions operator|(MoveOptions lhs, MoveOptions rhs);

const std::string STARTING_POSITION = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const uint8_t WHITE_KINGSIDE_CASTLING_RIGHTS_MASK	= 0b00000001;
const uint8_t WHITE_QUEENSIDE_CASTLING_RIGHTS_MASK	= 0b00000010;
const uint8_t BLACK_KINGSIDE_CASTLING_RIGHTS_MASK	= 0b00000100;
const uint8_t BLACK_QUEENSIDE_CASTLING_RIGHTS_MASK	= 0b00001000;
const uint8_t WHITE_CASTLING_RIGHTS_MASK			= 0b00000011;
const uint8_t BLACK_CASTLING_RIGHTS_MASK			= 0b00001100;
const uint8_t KINGSIDE_CASTLING_RIGHTS_MASK			= 0b00000101;
const uint8_t QUEENSIDE_CASTLING_RIGHTS_MASK		= 0b00001010;
const uint8_t CASTLING_RIGHTS_MASK					= 0b00001111;
const uint8_t IN_CHECK_MASK							= 0b00010000;

const Square WHITE_KING_STARTING_SQUARE = Square(0, 4);
const Square BLACK_KING_STARTING_SQUARE = Square(7, 4);
const Square WHITE_QUEENSIDE_ROOK_STARTING_SQUARE = Square(0, 0);
const Square WHITE_KINGSIDE_ROOK_STARTING_SQUARE = Square(0, 7);
const Square BLACK_QUEENSIDE_ROOK_STARTING_SQUARE = Square(7, 0);
const Square BLACK_KINGSIDE_ROOK_STARTING_SQUARE = Square(7, 7);



class Position {
private:
	// index these bitboard arrays with the enums defined above!
	std::array<Bitboard, 2> pieces_by_color; // array of bitboards, [0] for white pieces, [1] for black pieces
	std::array<Bitboard, 6> pieces_by_type; // array of bitboards, [0] pawns, [1] knights, [2] bishops, [3] rooks, [4] queens, [5] kings

	Square epsq; // bit board of where the en passant square is (if it exists) 

	uint16_t ply; // count of the current ply
	uint16_t ply_clock; // number of plys that have been played since the last capture or pawn move
	uint8_t flags;	// 4 bits for castling rights (0b000xxxx1-WK, 0b000xxx1x-WQ, 0b000xx1xx-BK, 0b000x1xxx-BQ)
					// 1 bit for in-check status (0b0001xxxx-Check, 0b0000xxxx-No Check)
					// 3 bonus bits!

	Bitboard checking_pieces; // bitboard of squares that are occupied by a piece which is checking the king
	Bitboard spying_pieces;	// bitboard of squares that are occupied by a piece which is "spying on" the king
	Bitboard pinned_pieces;	// bitboard of squares that are occupied by pinned pieces
	std::vector<Bitboard> check_vectors; // array of bitboards containing squares between checking pieces and the king
	std::vector<Bitboard> spy_vectors; // array of bitboards containing squares between spying pieces and the king


	std::vector<Move> move_gen_p(Square from);
	std::vector<Move> move_gen_k(Square from);
	std::vector<Move> move_gen_sliders(Square from, Types type);
	std::vector<Move> move_gen_generic(Square from, std::vector<Directions> directions, int max_distance = -1, MoveOptions move_opts = (MoveOptions::PLACE | MoveOptions::CAPT));

	// Implementing a simpler movegen algorithm in hopes that it will be more correct, and to aid in debugging. These methods are to support that effort.
	std::vector<Move> BASIC_pl_move_gen();
	std::vector<Move> BASIC_pl_std_move_gen(Square from);
	std::vector<Move> BASIC_pl_pawn_move_gen(Square from);
	std::vector<Move> BASIC_pl_castle_move_gen(Square from);

	friend class MoveIntegrityChecker;

public:
	// Constructors
	Position(std::string fen) { parse_fen(fen); }
	Position() : Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {} // default standard starting position for chess, in FEN notation

	// Methods
	void parse_fen(std::string fen);
	//TODO std::string gen_fen();
	std::vector<Move> move_gen();
	// TODO remove static keyword; I dont think it does what you think it does ;)
	static void disp_bitboard(Bitboard bb, std::string title, char piece_c, char empty_c);
	static void disp_bitboard(Bitboard bb, std::string title);
	static void disp_bitboard(Bitboard bb);
	Colors get_turn();
	Types get_type(Square sq);
	Square get_king_square(Colors color);
	bool is_type(Square sq, Types type);
	bool is_open(Square sq);
	bool is_opponent(Square sq);
	bool is_capture_move(Move move);
	bool is_friend(Square sq);
	bool is_castling_legal(CastleSide side);
	bool K_castling_right();
	bool Q_castling_right();
	void set_castling_right(Colors color, CastleSide side, bool set);
	bool get_castling_right(Colors color, CastleSide side);
	bool is_in_check();
	bool square_covered(Square sq, Colors attacker);
	std::vector<Square> find_attackers(Square sq, Types piece_type, Colors attacker);
	void king_threats();
	void set_in_check(bool set);
	void disp_bitboards();
	void disp_castling();
	void disp_epsq();
	void disp_plys();
	void disp_move(Move move);
	void disp(bool show_all = false);
	Position& make_move(Move move);
	Position& undo();
	//void perft(unsigned int depth, perft_moves& counts);
	//void perft_core(unsigned int depth, perft_moves& counts);
	Bitboard get_occupied();
	void check_integrity();
	Position& mirror_position();

	// Implementing a simpler movegen algorithm in hopes that it will be more correct, and to aid in debugging. These methods are to support that effort.
	std::vector<Move> BASIC_move_gen();
	void BASIC_king_threats(); // the basic one will only calculate if the king is in check.
	bool is_position_illegal(); // calculates whether the opponent's king can be captured from this position, which implies the last move was illegal.

};

class MoveIntegrityChecker {

	int friendly_piece_count_before;
	int enemy_piece_count_before;
	int rook_piece_count_before;
	int knight_piece_count_before;
	int queen_piece_count_before;
	int king_piece_count_before;
	int pawn_piece_count_before;
	int bishop_piece_count_before;
	uint64_t covered_squares_before;

	int friendly_piece_count_after;
	int enemy_piece_count_after;
	int rook_piece_count_after;
	int knight_piece_count_after;
	int queen_piece_count_after;
	int king_piece_count_after;
	int pawn_piece_count_after;
	int bishop_piece_count_after;
	uint64_t covered_squares_after;

	void get_before_stats(Position pos, Colors turn);
	void get_after_stats(Position pos, Colors turn);

public:
	MoveIntegrityChecker(Position pos, Colors turn) { get_before_stats(pos, turn); };
	void check_castling_rights(Position pos);
	void check_move_integrity(Position pos, Colors turn, Move move);
};