#include <string>
#include <iostream>
#include <bitset>
#include <utility>
#include <assert.h>
#include "Move.h"
#include "Position.h"
#include "Types.h"
#include "Colors.h"
#include "Utils.h"

// map from piece types to valid move directions for each piece
const std::vector<std::vector<int>> move_directions = {
	{-8, 8}, // Pawn
	{-17, -15, -10, -6, 6, 10, 15, 17}, // Knight
	{-9, -7, 7, 9}, // Rook
	{-8, -1, 1, 8}, // Bishop
	{-9, -8, -7, -1, 1, 7, 8, 9}, // Queen
	{-9, -8, -7, -1, 1, 7, 8, 9} // King
};

const std::vector<std::vector<int>> pawn_capt_directions = {
	{-9, -7}, // White
	{7, 9} // Black
};

// map of move directions to bitmasks for the move generation
std::map<int, uint64_t> move_masks = {
	{-17, ~0x010101010101ffff},
	{-15, ~0x808080808080ffff},
	{-10, ~0x03030303030303ff},
	{-9, ~0x01010101010101ff},
	{-8, ~0x00000000000000ff},
	{-7, ~0x80808080808080ff},
	{-6, ~0xc0c0c0c0c0c0c0ff},
	{-2, ~0x0303030303030303},
	{-1, ~0x0101010101010101},
	{1, ~0x8080808080808080},
	{2, ~0xc0c0c0c0c0c0c0c0},
	{6, ~0xff03030303030303},
	{7, ~0xff01010101010101},
	{8, ~0xff00000000000000},
	{9, ~0xff80808080808080},
	{10, ~0xffc0c0c0c0c0c0c0},
	{15, ~0xffff010101010101},
	{17, ~0xffff808080808080}
};

MoveOptions operator|(MoveOptions lhs, MoveOptions rhs) {
	return static_cast<MoveOptions> (int (lhs) | int (rhs));
}

void Position::parse_fen(std::string fen) {
	//TODO: implement a way to backup the values so if a parse error occurs, we can revert the position.

	unsigned int sq_counter = 0, char_index = 0, file, rank;
	bool turn;
	char c, file_c, rank_c;
	std::string s;
	uint64_t cur_sq;
	Colors color;
	Types type;

	// clean slate bitboards
	for (auto& bitboard : pieces_by_color) { bitboard = 0; }
	for (auto& bitboard : pieces_by_type) { bitboard = 0; }
	epsq = 0;

	// clean slate flags
	flags = 0;  // Unset the flags to clear the slate

	while (sq_counter < 64) {								// iterate over characters that "setup" the board
		c = fen[char_index++];								// consume a character

		if (isdigit(c)) {									// if char is a digit, then there are empty squares
			unsigned int empty_sqs = c - '0';				// subtract ascii value of '0' from c. Ex. if c is '7' then this will be 55 - 48 = 7.
			if (empty_sqs < 1 || empty_sqs > 8) {			// only 1-8 are valid
				// TODO: exception case - not a recognized number of empty squares!
				return;
			}
			sq_counter += empty_sqs;						// skip the empty squares and do nothing with them
		}
		else if (isalpha(c)) {								//if char is alphabetical, then it must be a piece.

			if (isupper(c)) { color = Colors::WHITE; }		// uppercase letters are for white
			else			{ color = Colors::BLACK; }		// lowercase for black
			c = tolower(c);									// convert to lowercase to simplify processing
															// each character corresponds to a piece type
			if		(c == 'p') { type = Types::PAWN; }
			else if (c == 'n') { type = Types::KNIGHT; }
			else if (c == 'b') { type = Types::BISHOP; }
			else if (c == 'r') { type = Types::ROOK; }
			else if (c == 'q') { type = Types::QUEEN; }
			else if (c == 'k') { type = Types::KING; }
			else {
				// TODO: exception case - not a recognized piece!
				return;
			}
			cur_sq = 1ULL << sq_counter;					// generate bitboard of the current square
			pieces_by_color[color] |= cur_sq;			// mark the square were on with the color of the piece we parsed
			pieces_by_type[type] |= cur_sq;			// mark the square were on with the piece type we parsed

			++sq_counter;									// increment the square counter to the next square
		}
		else if (c == '/') {
			// Currently there is no reason to do anything here, since we just ignore this character. However, should check that it was not misplaced for validity.
		}
		else if (c == ' ') { // Finished parsing the board setup portion
			break;
		}
	}

	while ((c = fen[char_index++]) == ' '); // Consume spaces

		if (c == 'w') { turn = true; }
		else if (c == 'b') { turn = false; }
		else {
			// TODO: Exception case - not a recognized player!
			return;
		}

	while ((c = fen[char_index++]) == ' '); // Consume spaces

	if (c == '-') {} // If the castling string is a dash, we can ignore it and move on
	else {
		do {
			if (c == 'K') { flags |= (1 << 0); } // Set White Kingside castle
			else if (c == 'Q') { flags |= (1 << 1); } // Set White Queenside castle
			else if (c == 'k') { flags |= (1 << 2); } // Set Black Kingside castle
			else if (c == 'q') { flags |= (1 << 3); } // Set Black Queenside castle
			else {
				// TODO: Exception case - not a recognized castling right!
				return;
			}
		} while ((c = fen[char_index++]) != ' '); // Consume characters until there is a space
	}

	while ((c = fen[char_index++]) == ' '); // Consume spaces

	if (c == '-') {} // again, ignore and move on
	else { // otherwise...
		// parse the square in algebraic notation
		file_c = c;
		rank_c = fen[char_index++];
		// compute the numeric rank and file
		file = file_c - 'a';
		rank = rank_c - '1';
		if (file >= 8 || rank >= 8) { // if either the rank or file is out of bounds...
			// TODO: Exception case - parsed algebraic notation is not on board!
			return;
		}
		else { // otherwise compute the bitboard of the en passant square
			epsq = 1ULL << ((7 - rank) * 8 + file); // (7 - rank) flips the numbering of the ranks to match our convention. (result * 8) then gets the row offsets. (result + file) then adds the column offsets.
		}
	}

	while ((c = fen[char_index++]) == ' '); // Consume spaces

	do {
		if (!isdigit(c)) {
			// TODO: Exception case - not a numeric value!
			return;
		}
		s.append(1, c); // append the digit characters to the string
	} while ((c = fen[char_index++]) != ' '); // get the next character and check it against the space constant
	ply_clock = std::stoi(s); // convert the string to an integer and store
	s.clear();

	while ((c = fen[char_index++]) == ' '); // Consume spaces

	do {
		if (!isdigit(c)) {
			// TODO: Exception case - not a numeric value!
			return;
		}
		s.append(1, c); // append the digit characters to the string
	} while ((c = fen[char_index++]) != '\0'); // get the next character and check it against the end-of-string constant
	ply = std::stoi(s) * 2 - (turn ? 1 : 0); // calculate the ply from the full turn count and whose move it is
	
	//TODO: check to see if the king is in check, and set the in-check bits appropriately

	return;
}

void Position::disp_bitboard(uint64_t bitboard, std::string title, char piece_c, char empty_c) {
	if (title != "") {
		std::cout << title << "\n";
	}
	unsigned int bit = 0;
	std::cout << "-------------------";
	std::cout << "\n";
	for (int i = 0; i < 8; ++i) {
		std::cout << '|';
		std::cout << ' ';
		for (int j = 0; j < 8; ++j) {
			if ((bitboard & (1ULL << bit)) > 0) {
				std::cout << piece_c;
			}
			else {
				std::cout << empty_c;
			}
			std::cout << ' ';
			++bit;
		}
		std::cout << '|';
		std::cout << '\n';
	}
	std::cout << "-------------------";
	std::cout << '\n';
}

void Position::disp_bitboard(uint64_t bitboard, std::string title) {
	disp_bitboard(bitboard, title, 'X', ' ');
}

void Position::disp_bitboard(uint64_t bitboard) {
	disp_bitboard(bitboard, "");
}

// re-implement these so that they actually determine if legal castling can be done
bool Position::K_castle_right()
{
	uint8_t cast_flag, mask = 0b00000001;

	(get_turn()) ? (cast_flag = ((flags >> 2) & mask)) : (cast_flag = (flags & mask));

	return (bool) cast_flag;
}

bool Position::Q_castle_right()
{
	uint8_t cast_flag, mask = 0b00000010;

	(get_turn()) ? (cast_flag = ((flags >> 2) & mask)) : (cast_flag = (flags & mask));

	return (bool) cast_flag;
}

void Position::disp_bitboards() {

	std::array<std::string, 2> colors = { "White:", "Black:" };
	std::array<char, 2> color_cs = { 'W', 'B' };
	std::array<std::string, 6> types = { "Pawns:","Knights:","Bishops:","Rooks:","Queens:","Kings:" };
	std::array<char, 6> pcs = { 'P', 'N', 'B', 'R', 'Q', 'K' };

	int i = 0;
	std::cout << "\nPieces by Color\n";
	for (auto bitboard : this->pieces_by_color) {
		disp_bitboard(bitboard, colors[i], color_cs[i], ' ');
		++i;
	}
	i = 0;
	std::cout << "\nPieces by Type\n";
	for (auto bitboard : this->pieces_by_type) {
		disp_bitboard(bitboard, types[i], pcs[i], ' ');
		++i;
	}
}

void Position::disp_castling() {
	std::cout << std::boolalpha;
	std::cout << "\nCastling\n";
	std::cout << "White:\n";
	std::cout << "  Kingside - " << ((flags & 0b0001) > 0) << "\n";
	std::cout << "  Queenside - " << ((flags & 0b0010) > 0) << "\n";
	std::cout << "Black:\n";
	std::cout << "  Kingside - " << ((flags & 0b0100) > 0) << "\n";
	std::cout << "  Queenside - " << ((flags & 0b1000) > 0) << "\n";
}

void Position::disp_epsq() {
	bool available = (bool) epsq;
	std::string square_an = "";
	std::cout << std::boolalpha << "\nEn Passant\n";

	if (available) {
		ASSERT_ONE_SQUARE(epsq); // assert that epsq only contains a single 1-bit
		unsigned int maxbit = 0;
		while ((epsq >> (maxbit + 1)) > 0) maxbit++;

		unsigned int file_i = maxbit % 8;
		unsigned int rank_i = 8 - (maxbit / 8); // Dont worry, it truncates
		square_an.append(1, 'a' + file_i).append(1, '0' + rank_i);
	}
	else {
		square_an = "None";
	}
	std::cout << "  Available? - " << available << "\n";
	std::cout << "  En Passant Square - " << square_an << "\n";
}

void Position::disp_plys() {
	std::cout << "\nPlys\n";
	std::cout << "  Ply Count - " << ply << "\n";
	std::cout << "  50 Move Clock - " << ply_clock << "\n";
	std::cout << "  Turn - " << get_turn() << "\n";
}

void Position::disp() {
	uint64_t	bitboard,
				mask,
				all = pieces_by_color[WHITE] | pieces_by_color[BLACK];
	Colors color;
	Types type;
	std::string type_c = "PNBRQK";

	std::cout << std::endl << "Position" << std::endl;
	std::cout << "-------------------" << std::endl;

	for (unsigned int sq = 0; sq < 64; ++sq) {
		if (sq % 8 == 0) { std::cout << "|"; };
		mask = 1ULL << sq;
		if ((all & mask) > 0) { // if there is a piece in this square...
			color = WHITE;
			for (int i = 0; i < pieces_by_color.size(); ++i) {
				bitboard = pieces_by_color[i];
				if ((bitboard & mask) > 0) {
					color = static_cast<Colors> (i);
					break;
				}
			}
			type = NONE;
			for (int i = 0; i < pieces_by_type.size(); ++i) {
				bitboard = pieces_by_type[i];
				if ((bitboard & mask) > 0) {
					type = static_cast<Types> (i);
					break;
				}
			}
			std::cout << " " << (color == BLACK ? char (tolower(type_c[type])) : char (type_c[type]));
		}
		else {
		std::cout << "  ";
		}
		if (sq % 8 == 7) { std::cout << " |" << std::endl; };
	}
	std::cout << "-------------------" << std::endl;

	disp_castling();
	disp_epsq();
	disp_plys();
}

Colors Position::get_turn() {
	return static_cast<Colors>(((ply - 1) % 2));
}

Types Position::get_type(uint64_t square)
{
	ASSERT_ONE_SQUARE(square);
	for (int i = 0; i < pieces_by_type.size(); ++i) {
		auto pieces_of_type = pieces_by_type[i];
		if (square & pieces_of_type) { return static_cast<Types>(i); }
	}
	return NONE;
}

bool Position::on_pawn_start_rank(uint64_t square) {
	unsigned int n;
	!get_turn() ? n = 1 : n = 6;
	return on_nth_rank(square, n);
}

bool Position::on_promote_rank(uint64_t square) {
	unsigned int n;
	!get_turn() ? n = 7 : n = 0;
	return on_nth_rank(square, n);
}

std::vector<Move> Position::move_gen_generic(uint64_t from, std::vector<int> directions, unsigned int max_distance, MoveOptions move_opts) {
	uint64_t move_mask, to;
	int gen_dist;
	std::vector<Move> moves;
	Types from_type = get_type(from);

	assert(from_type != NONE); // make sure that the piece we are generating moves for isnt a none type
	assert((from & pieces_by_type[from_type]) != 0); // ensure that from is a piece of the correct type

	for (auto dir : directions) {	// loop over each given direction.
		assert(dir != 0);			// cannot allow a no-direction.

		to = from;						// init the move square to the from square.
		gen_dist = 0;					// init the distance checked so far.
		move_mask = move_masks[dir];	// get the correct move mask for this direction.

		// keep incrementing in the specified direction until either 1) the starting position is in the move mask or 2) the gen_distance has reached the max.
		while (((move_mask & to) != 0) && (gen_dist++ != max_distance)) {

			// if the direction is negative, we want to perform right-shifting, left-shifting for positive values.
			(dir < 0) ? (to >>= abs(dir)) : (to <<= dir);

			// the condition evaluates if the "to" square intersects with its own pieces. if so, this move is illegal and no more moves in this direction are legal.
			if (to & pieces_by_color[get_turn()]) {
				assert(get_type(to) != NONE);
				if (move_opts & MoveOptions::SELF_CAPT) {
					moves.push_back(Move(from, to, get_turn(), from_type, get_type(to)));
				}
				break;
			}

			// the condition evaluates if the "to" square intersects with the opponents pieces. if so, this move is a capture and no more moves in this direction are legal.
			if (to & pieces_by_color[!get_turn()]) {
				assert(get_type(to) != NONE); // make sure that we arent capturing a none type
				if (move_opts & MoveOptions::CAPT) {
					moves.push_back(Move(from, to, get_turn(), from_type, get_type(to)));
				}
				break;
			}

			// if neither previous condition evaluates true, then this move is legal and we can continue in this direction.
			if (move_opts & MoveOptions::PLACE) {
				moves.push_back(Move(from, to, get_turn(), from_type));
			}
		}
	}
	return moves;
}

bool Position::attacked_by_piece(Types piece_type, uint64_t from) {
	uint64_t mask, to;
	unsigned int	gen_dist,
					max_distance = -1;

	if (piece_type == PAWN || piece_type == KNIGHT || piece_type == KING) { max_distance = 1; }

	if (piece_type == PAWN) { auto directions = pawn_capt_directions[get_turn()]; }
	else { auto directions = move_directions[piece_type]; }

	for (auto dir : move_directions[piece_type]) {
		to = from;
		gen_dist = 0;
		mask = move_masks[dir];
		while (((mask & to) != 0) && (gen_dist++ != max_distance)) {
			(dir < 0) ? (to >>= abs(dir)) : (to <<= dir);
			if (to & pieces_by_color[!get_turn()]) { // found an opponent's piece
				if (get_type(to) == piece_type) {
					return true;
				}
			}
			else if (to & pieces_by_color[get_turn()]) { // found a friendly piece, which blocks enemy pieces!
				break;
			}
			// otherwise continue the search
		}
	}
	return false;
}

bool Position::square_covered(uint64_t from) {
	for (int i = 0; i < 6; ++i) {
		if (attacked_by_piece(static_cast<Types>(i), from)) {
			return true;
		}
	}
	return false;
}

void Position::set_in_check() {
	auto king_sq = pieces_by_color[get_turn()] & pieces_by_type[KING];
	if (square_covered(king_sq)) {
		flags |= (1 << 4); // set the "in-check" bit.
	}
	else {
		flags &= (~(1 << 4)); // unset the "in-check" bit.
	}
}

bool Position::in_check() {
	return flags & (1 << 4);
}

std::vector<Move> Position::move_gen_sliders(uint64_t from, Types type) {
	assert(type != PAWN); // function cannot be used with pawns as it will cause pawns to have a backwards move as well.

	unsigned int max_distance = -1;
	if (type == KNIGHT || type == KING) { max_distance = 1; };
	return move_gen_generic(from, move_directions[type], max_distance);
}

std::vector<Move> Position::move_gen_k(uint64_t from)
{
	assert((from & pieces_by_type[KING]) != 0); //ensure that this piece is a king
	std::vector<Move> moves, temp_moves;

	moves = move_gen_sliders(from, KING);

	if (!in_check()) { // if not in check...
		if (K_castle_right()) { // see if we have the right to castle kingside
			if (!square_covered(from << 1) && !square_covered(from << 2)) { // see if the conditions for castling are met
				temp_moves = move_gen_generic(from, { 2 }, 1); 
				assert(temp_moves.size() == 1);

				// update move to be the more specific move type of castling
				temp_moves[0].set_move_type(CASTLE).set_special(from >> 4); // changes to a castle move and adds the King's rook square to the special field
				moves.push_back(temp_moves[0]);
			}
		}
		if (Q_castle_right()) { // see if we have the right to castle queenside
			if (!square_covered(from >> 1) && !square_covered(from >> 2)) { // see if the conditions for castling are met
				temp_moves = move_gen_generic(from, { -2 }, 1);
				assert(temp_moves.size() == 1);

				temp_moves[0].set_move_type(CASTLE).set_special(from << 3); // changes to a castle move and adds the Queen's rook square to the special field
				moves.push_back(temp_moves[0]);
			}
		}
	}
	return moves;
}

std::vector<Move> Position::move_gen_p(uint64_t from) {
	ASSERT_ONE_SQUARE(from);
	std::vector<Move> moves, temp_moves;
	Move promote_move;
	unsigned int max_distance;
	Colors turn = get_turn();

	// Get pawn push moves
	(on_pawn_start_rank(from)) ? (max_distance = 2) : (max_distance = 1);
	moves = move_gen_generic(from, { move_directions[PAWN][turn] }, max_distance, MoveOptions::PLACE);

	// Get standard pawn capture moves
	temp_moves = move_gen_generic(from, pawn_capt_directions[turn], 1, MoveOptions::CAPT);
	moves.insert(moves.end(), temp_moves.begin(), temp_moves.end());

	// Check all generated moves to see if any are eligible for promotion
	int old_size = moves.size();
	for (int i = 0; i < old_size; ++i) {
		if (on_promote_rank(moves[i].get_to())) {
			promote_move = moves[i].set_move_type(PROMOTION);
			for (int j = 1; j < 5; ++j) {
				promote_move.set_promote_type(static_cast<Types>(i));
				if (j == 1) {
					moves[i] = promote_move; // overwrite the found move with the first promotion move
				}
				else {
					moves.push_back(promote_move); // append the remaining promotion moves
				}
			}
		}
	}

	// En Passant moves
	if (epsq) {
		ASSERT_ONE_SQUARE(epsq);
		if ((from >> 7) == epsq || (from << 9) == epsq) {
			moves.push_back(Move(from, epsq, from << 1, turn, PAWN, PAWN, NONE, EN_PASSANT));
		}
		if ((from >> 9) == epsq || (from << 7) == epsq) {
			moves.push_back(Move(from, epsq, from >> 1, turn, PAWN, PAWN, NONE, EN_PASSANT));
		}
	}
	return moves;
}

std::vector<Move> Position::move_gen()
{
	Colors turn = get_turn();
	uint64_t pieces, from;
	Types type;
	std::vector<Move> moves, temp_moves;

	set_in_check(); // set the check status for the position. Allows all other move generation to refer to a valid in_check bit

	for (int i = 0; i < pieces_by_type.size(); ++i) { // for each piece type...
		pieces = pieces_by_type[i] & pieces_by_color[turn]; // get the pieces of that type and of the right color
		type = static_cast<Types>(i); // calculate the type from the iteration var

		while ((from = (pieces & (~pieces + 1))) != 0) { // while the from square is not zero (calculates the next from square by isolated the least significant bit)
			pieces &= (~from); // remove the from square from the set of pieces
			switch (type) { // based on which type of piece this is...
				case PAWN: 
					temp_moves = move_gen_p(from); // generate pawn moves if its a pawn
					break;
				case KING:
					temp_moves = move_gen_k(from); // generate king moves if its a king
					break;
				default:
					temp_moves = move_gen_sliders(from, type); // generate all the other pieces moves if its neither
			}
			moves.insert(moves.end(), temp_moves.begin(), temp_moves.end()); // append the found moves to the end of the move list
		}
	}
	return moves;
}



//std::vector<Move> Position::move_gen() {
//	for (int i = 0; i < pieces_by_type.size(); ++i) {
//		auto pieces_of_type = pieces_by_type[i];
//
//	}
//}