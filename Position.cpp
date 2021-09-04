#include <string>
#include <iostream>
#include "Position.h"

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
	else {
		file_c = fen[char_index++];
		rank_c = fen[char_index++];
		file = file_c - 'a';
		rank = rank_c - '1';
		if (file >= 8 || rank >= 8) {
			// TODO: Exception case - parsed algebraic notation is not on board!
			return;
		}
		else {
			epsq = 1ULL << ((7 - rank) * 8 + file); // (7 - rank) flips the numbering of the ranks to match our convention. (result * 8) then gets the row offsets. (result + file) then adds the column offsets.
		}
	}

	while ((c = fen[char_index++]) == ' '); // Consume spaces

	while ((c = fen[char_index++]) != ' ') { // scan over numeric values
		if (!isdigit(c)) {
			// TODO: Exception case - not a numeric value!
			return;
		}
		s.append(1, c); // append the digit characters to the string
	}
	ply_clock = atoi(s.c_str()); // convert the string to an integer and store

	while ((c = fen[char_index++]) == ' '); // Consume spaces

	while ((c = fen[char_index++]) != ' ') { // scan over numeric values
		if (!isdigit(c)) {
			// TODO: Exception case - not a numeric value!
			return;
		}
		s.append(1, c); // append the digit characters to the string
	}
	ply = atoi(s.c_str()) * 2 - (turn ? 1 : 0); 
	
	//TODO: check to see if the king is in check, and set the in-check bits appropriately

	return;
}

void Position::display_bitboard(uint64_t bitboard) {
	unsigned int bit = 0;
	std::cout << "-------------------";
	std::cout << "\n";
	for (int i = 0; i < 8; ++i) {
		std::cout << '|';
		std::cout << ' ';
		for (int j = 0; j < 8; ++j) {
			if ((bitboard & (1ULL << bit)) > 0) {
				std::cout << 'X';
			}
			else {
				std::cout << ' ';
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

void Position::display_bitboard(uint64_t bitboard, std::string title) {
	std::cout << title << "\n";
	display_bitboard(bitboard);
}

void Position::display_bitboards() {
	std::array<std::string, 2> colors = { "White Pieces", "Black Pieces" };
	std::array<std::string, 6> types = { "Pawns","Knights","Bishops","Rooks","Queens","Kings" };
	int i = 0;
	for (auto bitboard : this->pieces_by_color) {
		this->display_bitboard(bitboard, colors[i++]);
	}
	i = 0;
	for (auto bitboard : this->pieces_by_type) {
		this->display_bitboard(bitboard, types[i++]);
	}
}