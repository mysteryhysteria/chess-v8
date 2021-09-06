#include <string>
#include <iostream>
#include <bitset>
#include <assert.h>
#include "Position.h"
#include "Types.h"
#include "Colors.h"

std::map<Types, std::vector<int>> move_directions = {
	{PAWN, {-8, 8}},
	{KNIGHT, {-17, -15, -10, -6, 6, 10, 15, 17}},
	{BISHOP, {-9, -7, 7, 9}},
	{ROOK,  {-8, -1, 1, 8}},
	{QUEEN, {-9, -8, -7, -1, 1, 7, 8, 9}},
	{KING, {-9, -8, -7, -1, 1, 7, 8, 9}}
};

// 
std::map<int, uint64_t> move_masks = {
	{-17, 0x010101010101ffff},
	{-15, 0x808080808080ffff},
	{-10, 0x03030303030303ff},
	{-9, 0x01010101010101ff},
	{-8, 0x00000000000000ff},
	{-7, 0x80808080808080ff},
	{-6, 0xc0c0c0c0c0c0c0ff},
	{-1, 0x0101010101010101},
	{1, 0x8080808080808080},
	{6, 0xff03030303030303},
	{7, 0xff01010101010101},
	{8, 0xff00000000000000},
	{9, 0xff80808080808080},
	{10, 0xffc0c0c0c0c0c0c0},
	{15, 0xffff010101010101},
	{17, 0xffff808080808080}
};

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
		assert(std::bitset<64>(epsq).count() == 1); // assert that epsq only contains a single 1-bit
		unsigned int maxbit = 0;
		while ((epsq >> maxbit + 1) > 0) maxbit++;

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
			for (int i = 0; i < pieces_by_color.size(); ++i) {
				bitboard = pieces_by_color[i];
				if ((bitboard & mask) > 0) {
					color = static_cast<Colors> (i);
					break;
				}
			}
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

//std::vector<Move> Position::move_gen() {
//	for (int i = 0; i < pieces_by_type.size(); ++i) {
//		auto pieces_of_type = pieces_by_type[i];
//
//	}
//}