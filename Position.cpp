#include "Position.h"

// TODO: move to header file
const std::vector<std::vector<Directions>> move_directions = { // map from piece types to valid move directions for each piece
	{Directions::N,	  Directions::S}, // Pawns
	{Directions::NNE, Directions::ENE, Directions::ESE, Directions::SSE, Directions::SSW, Directions::WSW, Directions::WNW, Directions::NNW }, // Knight
	{Directions::NE,  Directions::SE,  Directions::SW,  Directions::NW }, // Bishop
	{Directions::N,   Directions::E,   Directions::S,   Directions::W },  // Rook
	{Directions::N,   Directions::NE,  Directions::E,   Directions::SE,  Directions::S,   Directions::SW,  Directions::W,   Directions::NW }, // Queen
	{Directions::N,   Directions::NE,  Directions::E,   Directions::SE,  Directions::S,   Directions::SW,  Directions::W,   Directions::NW } // King
};

// TODO: move to header file
const std::vector<std::vector<Directions>> pawn_capt_directions = {
	{Directions::WHITE_PAWN_CAPTURE_WEST, Directions::WHITE_PAWN_CAPTURE_EAST}, // White
	{Directions::BLACK_PAWN_CAPTURE_WEST, Directions::BLACK_PAWN_CAPTURE_EAST} // Black
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
	std::optional<Square> cur_sq = Square(1); // initialize to the first square
	Colors color;
	Types type;

	// clean slate bitboards
	for (auto& bb : pieces_by_color) { bb = Bitboard(); };
	for (auto& bb : pieces_by_type) { bb = Bitboard(); };
	epsq = Square();

	// clean slate flags
	flags = 0;  // Unset the flags to clear the slate

	while (sq_counter < 64 && cur_sq.has_value()) {								// iterate over characters that "setup" the board
		c = fen[char_index++];								// consume a character

		if (isdigit(c)) {									// if char is a digit, then there are empty squares
			unsigned int empty_sqs = c - '0';				// subtract ascii value of '0' from c. Ex. if c is '7' then this will be 55 - 48 = 7.
			if (empty_sqs < 1 || empty_sqs > 8) {			// only 1-8 are valid
				// TODO: exception case - not a recognized number of empty squares!
				return;
			}
			sq_counter += empty_sqs;						// skip the empty squares and do nothing with them
			cur_sq = cur_sq->next(empty_sqs);
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

			pieces_by_color[color].mark_square(cur_sq.value());		// mark the square were on with the color of the piece we parsed
			pieces_by_type[type].mark_square(cur_sq.value());		// mark the square were on with the piece type we parsed
			cur_sq = cur_sq->next();								// get the next square

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
			epsq = Square(rank, file); // Sets the en passant square to the computed rank and file
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
	
	BASIC_king_threats();

	return;
}

void Position::disp_bitboard(Bitboard bitboard, std::string title, char piece_c, char empty_c) {
	if (title != "") {
		std::cout << title << "\n";
	}
	std::optional<Square> test_sq = Square().first();
	std::cout << "-------------------";
	std::cout << "\n";
	for (int i = 0; i < 8; ++i) {
		std::cout << '|';
		std::cout << ' ';
		for (int j = 0; j < 8; ++j) {
			if (bitboard.contains(test_sq.value())) {
				std::cout << piece_c;
			}
			else {
				std::cout << empty_c;
			}
			std::cout << ' ';
			test_sq = test_sq->next();
		}
		std::cout << '|';
		std::cout << '\n';
	}
	std::cout << "-------------------";
	std::cout << '\n';
}

void Position::disp_bitboard(Bitboard bitboard, std::string title) {
	disp_bitboard(bitboard, title, 'X', ' ');
}

void Position::disp_bitboard(Bitboard bitboard) {
	disp_bitboard(bitboard, "");
}

bool Position::is_castling_legal(CastleSide side) {
	Square king_sq = pieces_by_color[get_turn()] & pieces_by_type[Types::KING];
	std::vector<Square> be_empty, be_safe;
	Bitboard all_pieces = pieces_by_color[Colors::WHITE] | pieces_by_color[Colors::BLACK];

	assert(side == CastleSide::QUEENSIDE || side == CastleSide::KINGSIDE);

	// calculate the empty and safe squares based on which way you are castling
	if (side == CastleSide::QUEENSIDE) {
		be_empty = { Square(king_sq >> 1), Square(king_sq >> 2), Square(king_sq >> 3) };
		be_safe = { Square(king_sq >> 1), Square(king_sq >> 2) };
	}
	else if (side == CastleSide::KINGSIDE) {
		be_empty = { Square(king_sq << 1), Square(king_sq << 2) };
		be_safe = be_empty;
	}

	// test the empty squares for emptiness
	for (auto& empty_sq : be_empty) {
		if (all_pieces.contains(empty_sq)) {
			return false;
		}
	}

	// test the safe squares for safeness
	for (auto& safe_sq : be_safe) {
		if (square_covered(safe_sq, !get_turn())) {
			return false;
		}
	}

	// all good!
	return true;
}

bool Position::get_castling_right(Colors color, CastleSide side) {
	uint8_t shift, mask;
	switch (color) {
	case Colors::WHITE:
		shift = 0; break;
	case Colors::BLACK:
		shift = 2; break;
	}

	switch (side) {
	case CastleSide::KINGSIDE:
		mask = 0b00000001; break;
	case CastleSide::QUEENSIDE:
		mask = 0b00000010; break;
	}

	return static_cast<bool>((flags >> shift) & mask);
}

// re-implement these so that they actually determine if legal castling can be done
bool Position::K_castling_right()
{
	uint8_t cast_flag, mask = 0b00000001;

	(get_turn()) ? (cast_flag = ((flags >> 2) & mask)) : (cast_flag = (flags & mask));

	return (bool) cast_flag;
}

bool Position::Q_castling_right() {
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
	bool available = !epsq.is_empty();
	std::string square_an = "";
	std::cout << std::boolalpha << "\nEn Passant\n";

	if (available) {
		ASSERT_ONE_SQUARE(epsq); // assert that epsq only contains a single 1-bit
		unsigned int index, file_i, rank_i;
		index = epsq.convert_to_index();
		file_i = index % 8;
		rank_i = 8 - (index / 8); // Dont worry, it truncates
		square_an.append(1, 'a' + file_i).append(1, '0' + rank_i);
	}
	else {
		square_an = "None";
	}
	std::cout << "  Available? - " << available << std::endl;
	std::cout << "  En Passant Square - " << square_an << std::endl;
}

void Position::disp_plys() {
	std::cout << std::endl << "Plys" << std::endl;
	std::cout << "  Ply Count - " << ply << std::endl;
	std::cout << "  50 Move Clock - " << ply_clock << std::endl;
	std::cout << "  Turn - " << get_turn() << std::endl;
}

// This function will display each move in the PGN Short Algebraic Notation format.
void Position::disp_move(Move move) {
	SpecialMoves move_type = move.get_move_type();
	Types piece_type = move.get_type();
	Types capt_type = move.get_capt_type();
	Square from = move.get_from();
	Square to = move.get_to();
	Colors player = move.get_color();
	std::vector<Square> attackers;
	
	// special case for castling
	if (move_type == SpecialMoves::CASTLE) { // if the move is a castling move...
		std::cout << "O-O";
		if (from + Directions::KING_QUEENSIDE_CASTLE == to) { // if it is a queenside castle...
			std::cout << "-O";
		}
	}
	else {
		// Label moved piece.
		// pawns do not get a label
		if (piece_type != Types::PAWN) { // if the moved piece is not a pawn:
			std::cout << TypestoChar(piece_type); // output the letter representing this piece
		}
		
		// Disambiguate moves.
		// pawn captures are always disambiguated with the file
		if (piece_type == Types::PAWN && capt_type != Types::NONE) { // if the move is a pawn capture:
			std::cout << from.p2an()[0]; // output the file of the pawn
		}
		else {
			// Determine if disambiguation is necessary at all.
			// find all other pieces of the same type as the moved piece which could move to the destination square.
			attackers = find_attackers(to, piece_type, player);

			if (attackers.size() > 0) { // if there are any pieces found...
				// then compare the file of each found piece to the file of the starting square of the moved piece.
				bool unique_file = true;
				char start_file = from.p2an()[0];
				for (auto attacker : attackers) {
					if (attacker.p2an()[0] == start_file) { unique_file = false; };
				}
				if (unique_file) { // if none of the found pieces' files match the moved piece...
					std::cout << start_file; // print the starting file of the moved piece to disambiguate the move.
				}
				else { // if comparing files is not sufficient to disambiguate the move...
					// then compare the rank of each found piece to the rank of the original square of the moved piece
					bool unique_rank = true;
					char start_rank = from.p2an()[1];
					for (auto attacker : attackers) {
						if (attacker.p2an()[1] == start_rank) { unique_rank = false; };
					}
					if (unique_rank) { // if none of the found pieces' ranks match the moved piece...
						std::cout << start_rank; // print the starting rank of the moved piece to disambiguate the move.
					}
					else { // if neither comparing files nor comparing ranks is sufficient to disambiguate the move...
						std::cout << start_file << start_rank; // print both the starting file and rank.
					}
				}
			}
		}
		if (capt_type != Types::NONE) { // if this is a capture move...
			std::cout << 'x'; // print the 'x' character to denote the move is a capture.
		}
		std::cout << to.p2an(); // print the destination square.
		if (move_type == SpecialMoves::EN_PASSANT) { // if the move is an en passant capture...
			std::cout << " e.p."; // label the move as such.
		}
		else if (move_type == SpecialMoves::PROMOTION) { // if the move is a promotion...
			std::cout << '=' << TypestoChar(capt_type); // show the promotion piece.
		}
	}
	// TODO: implement the rest of this stuff.
	// if the move results in a check:
		// if the move results in a checkmate:
			// print '#'.
		// else:
			// print '+'.
			// if the move results in a double check:
				// print '+'.
			// end if.
		// end if.
	// end if.
	std::cout << std::endl;
}

void Position::set_castling_right(Colors color, CastleSide side, bool set) {
	if (set) {
		flags |= (0b0001 << (int(color) * 2 + int(side)));
	}
	else {
		flags &= ~(0b0001 << (int(color) * 2 + int(side)));
	}
}

void Position::disp(bool show_all) {
	Bitboard	bitboard,
				all = pieces_by_color[Colors::WHITE] | pieces_by_color[Colors::BLACK];
	std::optional<Square> test_sq = Square().first();
	Colors color;
	Types type;
	std::string type_c = "PNBRQK";

	std::cout << std::endl << "Position" << std::endl;
	std::cout << "-------------------" << std::endl;

	for (unsigned int sq = 0; sq < 64; ++sq) {
		if (sq % 8 == 0) { std::cout << "|"; };
		if (all.contains(test_sq.value())) { // if there is a piece in this square...
			color = Colors::WHITE;
			for (int i = 0; i < pieces_by_color.size(); ++i) {
				if (pieces_by_color[i].contains(test_sq.value())) {
					color = static_cast<Colors> (i);
					break;
				}
			}
			type = Types::NONE;
			for (int i = 0; i < pieces_by_type.size(); ++i) {
				if (pieces_by_type[i].contains(test_sq.value())) {
					type = static_cast<Types> (i);
					break;
				}
			}
			std::cout << " " << (color == Colors::BLACK ? char (tolower(type_c[type])) : char (type_c[type]));
		}
		else {
		std::cout << "  ";
		}
		if (sq % 8 == 7) { std::cout << " |" << std::endl; };
		test_sq = test_sq->next();
	}
	std::cout << "-------------------" << std::endl;

	if (show_all) {
		disp_bitboard(pinned_pieces, "Pinned Pieces");
		disp_bitboard(checking_pieces, "Checking Pieces");
		Bitboard threats;
		for (auto& vector : check_vectors) { threats |= vector; };
		disp_bitboard(threats, "Check Vectors");
		disp_bitboard(spying_pieces, "Spying Pieces");
		for (auto& vector : spy_vectors) { threats |= vector; };
		disp_bitboard(threats, "Spy Vectors");
		disp_castling();
		disp_epsq();
		disp_plys();
	}
}

Position& Position::make_move(Move move) {
	auto move_type = move.get_move_type();
	Colors turn = get_turn();
	Types type = move.get_type();
	Types capt_type = move.get_capt_type();
	Types promote_type = move.get_promote_type();
	Square from = move.get_from();
	Square to = move.get_to();
	Square special = move.get_special();

	// to square cannot be a king
	assert((to.get_u64() & pieces_by_type[Types::KING].get_u64()) == 0ULL);
	
	if (capt_type == Types::NONE) {
		// the to square must be empty.
		assert((to.get_u64() & get_occupied().get_u64()) == 0ULL);
	}
	else { // is a capture
		if (move_type == SpecialMoves::EN_PASSANT) {
			// if there is a capture type and the move_type is en_passant, the destination square must be empty.
			assert((to.get_u64() & get_occupied().get_u64()) == 0ULL);
		}
		else {
			// the to square must be occupied.
			assert((to.get_u64() & get_occupied().get_u64()) != 0ULL);
			// the to square must contain the capture type.
			assert((to.get_u64() & pieces_by_type[capt_type].get_u64()) != 0ULL);
			// the to square must be an opponent.
			assert((to.get_u64() & pieces_by_color[!turn].get_u64()) != 0ULL);
		}
	}
	
	// from square must contain a piece
	assert((from.get_u64() & get_occupied().get_u64()) != 0ULL);
	// from square must contain a piece of the correct color
	assert((from.get_u64() & pieces_by_color[turn].get_u64()) != 0ULL);
	// from square must contain a piece of the correct type
	assert((from.get_u64() & pieces_by_type[type].get_u64()) != 0ULL);

	if (move_type == SpecialMoves::EN_PASSANT) {
		// the special square must contain a piece
		assert((special.get_u64() & get_occupied().get_u64()) != 0ULL);
		// the special square must be a pawn
		assert((special.get_u64() & pieces_by_type[Types::PAWN].get_u64()) != 0ULL);
		// the special square must be an opponent
		assert((special.get_u64() & pieces_by_color[!turn].get_u64()) != 0ULL);
	}
	else if (move_type == SpecialMoves::CASTLE) {
		// special square must contain a piece
		assert((special.get_u64() & get_occupied().get_u64()) != 0ULL);
		// special square must be a rook
		assert((special.get_u64() & pieces_by_type[Types::ROOK].get_u64()) != 0ULL);
		// special square must be a friend
		assert((special.get_u64() & pieces_by_color[turn].get_u64()) != 0ULL);
	}	

	// create integrity checker
	MoveIntegrityChecker oracle = MoveIntegrityChecker(*this, turn);

	// Update the En Passant square state
	epsq = Square(); // clear the ep square by default
	Directions double_push_dir, single_push_dir;

	switch (turn) {
	case Colors::WHITE:
		double_push_dir = Directions::WHITE_PAWN_DOUBLE_PUSH;
		single_push_dir = Directions::WHITE_PAWN_PUSH;
		break;
	case Colors::BLACK:
		double_push_dir = Directions::BLACK_PAWN_DOUBLE_PUSH;
		single_push_dir = Directions::BLACK_PAWN_PUSH;
		break;
	}

	if (type == Types::PAWN && from.on_pawn_start_rank(turn) && from + double_push_dir == to) {
		epsq = from + single_push_dir;
	}

	// update the castling rights
	if ((flags & CASTLING_RIGHTS_MASK) != 0) { //Check if any castling rights remain
		
		// integrity check
		oracle.check_castling_rights(*this);

		// Test for King and Rook moves, as they may remove castling rights
		if (type == Types::KING) { // includes both normal moves and castling
			this->set_castling_right(turn, CastleSide::KINGSIDE, false);
			this->set_castling_right(turn, CastleSide::QUEENSIDE, false);
		}
		else if (type == Types::ROOK) { // rook moves
			if (from.on_nth_file(0)) { // if this is the a-file rook (either a1 or a8)
				this->set_castling_right(turn, CastleSide::QUEENSIDE, false);
			}
			else if (from.on_nth_file(7)) { // if this is the h-file rook (either h1 or h8)
				this->set_castling_right(turn, CastleSide::KINGSIDE, false);
			}
		}

		if (capt_type == Types::ROOK) {
			if (to.on_nth_file(0)) {
				this->set_castling_right(!turn, CastleSide::QUEENSIDE, false);
			}
			else if (to.on_nth_file(7)) {
				this->set_castling_right(!turn, CastleSide::KINGSIDE, false);
			}
		}
	}


	// update the pieces bitboards
	switch (move_type) {
	case SpecialMoves::STD:
		// remove moved piece from old square
		pieces_by_type[type].clear_square(from);
		pieces_by_color[turn].clear_square(from);

		// remove captured piece from new square
		if (capt_type != Types::NONE) {
			pieces_by_type[capt_type].clear_square(to);
			pieces_by_color[!turn].clear_square(to);
		}

		// place moved piece onto new square
		pieces_by_type[type].mark_square(to);
		pieces_by_color[turn].mark_square(to);

		break;

	case SpecialMoves::EN_PASSANT:
		// remove moved piece from old square
		pieces_by_type[Types::PAWN].clear_square(from);
		pieces_by_color[turn].clear_square(from);

		// remove captured piece from the special square
		pieces_by_type[Types::PAWN].clear_square(special);
		pieces_by_color[!turn].clear_square(special);

		// place moved piece onto en passant square
		pieces_by_type[Types::PAWN].mark_square(to);
		pieces_by_color[turn].mark_square(to);

		break;

	case SpecialMoves::PROMOTION:
		// remove moved piece from old square
		pieces_by_type[Types::PAWN].clear_square(from);
		pieces_by_color[turn].clear_square(from);

		// remove captured piece from new square
		if (capt_type != Types::NONE) {
			pieces_by_type[capt_type].clear_square(to);
			pieces_by_color[!turn].clear_square(to);
		}

		// place promoted piece onto new square
		pieces_by_type[promote_type].mark_square(to);
		pieces_by_color[turn].mark_square(to);
		break;

	case SpecialMoves::CASTLE:
		// remove king from starting square
		pieces_by_type[Types::KING].clear_square(from);
		pieces_by_color[turn].clear_square(from);
		
		// place king onto the new square
		pieces_by_type[Types::KING].mark_square(to);
		pieces_by_color[turn].mark_square(to);

		// remove rook from the special square
		pieces_by_type[Types::ROOK].clear_square(special);
		pieces_by_color[turn].clear_square(special);

		// place rook next to the king
		Directions rook_castle_dir;
		if (special.on_nth_file(0)) {
			rook_castle_dir = Directions::ROOK_QUEENSIDE_CASTLE;
		}
		else if (special.on_nth_file(7)) {
			rook_castle_dir = Directions::ROOK_KINGSIDE_CASTLE;
		}
		else {
			assert(false);
		}

		pieces_by_type[Types::ROOK].mark_square(special + rook_castle_dir);
		pieces_by_color[turn].mark_square(special + rook_castle_dir);
		break;

	}

	// integrity check
	oracle.check_move_integrity(*this, turn, move);

	// calculate if the opponent is in check
	set_in_check(square_covered(get_king_square(!turn), turn));

	// update the ply clock
	if (type == Types::PAWN || capt_type != Types::NONE) {
		ply_clock = 0; // reset if a pawn moves or a capture is made
	}
	else {
		ply_clock++;
	}

	// update the ply
	++ply;

	// move integrity check
	if (capt_type == Types::NONE) {
		
	}

	// board integrity check
	check_integrity();

	// return the position object
	return *this;
}

//void Position::perft(unsigned int depth, perft_moves& counts) {
//	this->perft_core(depth, counts);
//	std::cout << "Perft, depth " << depth << ": " << std::endl;
//	std::cout << "Move count: " << counts.moves << std::endl;
//	std::cout << "Capture count: " << counts.capts << std::endl;
//	std::cout << "En Passant count: " << counts.eps << std::endl;
//	std::cout << "Castles count: " << counts.castles << std::endl;
//	std::cout << "Promotions count: " << counts.promos << std::endl;
//	std::cout << "Checks count: " << counts.checks << std::endl << std::endl;
//	counts = { 0, 0, 0, 0, 0, 0 };
//}
//
//void Position::perft_core(unsigned int depth, perft_moves& counts) {
//	if (depth == 0) {
//		counts.moves++;
//		//std::cout << "Move # " << counts.moves << std::endl;
//		//disp_move_history(move_history);
//		//std::cout << std::endl;
//		//disp();
//		if (is_in_check()) {
//			counts.checks++;
//			//std::cout << "CHECK #" << counts.checks << ":" << std::endl;
//			//disp_move_history(move_history);
//			//std::cout << std::endl;
//			//disp();
//		}
//		auto prev_move = move_history.back();
//		switch (prev_move.get_move_type()) {
//			case STD:
//				if (prev_move.get_capt_type() != NONE) {
//					counts.capts++;
//				}
//				break;
//			case EN_PASSANT:
//				counts.capts++;
//				counts.eps++;
//				//std::cout << "EP #" << counts.eps << ":" << std::endl;
//				//disp_move_history(move_history);
//				//std::cout << std::endl;
//				break;
//			case PROMOTION:
//				counts.promos++;
//				break;
//			case CASTLE:
//				counts.castles++;
//				break;
//		}
//	}
//	else {
//		int sum = 0;
//		//this->disp();
//		for (auto& move : this->move_gen()) {
//			//std::cout << move;
//			this->make_move(move);
//			this->king_threats();
//			this->perft_core(depth-1, counts);
//			this->undo();
//		}
//	}
//}

Bitboard Position::get_occupied()
{
	return pieces_by_color[Colors::WHITE] | pieces_by_color[Colors::BLACK];
}

void Position::check_integrity() {
	// Piece sets must have the same pieces in them.
	assert(((pieces_by_color[Colors::WHITE] | pieces_by_color[Colors::BLACK])).get_u64() ==	((pieces_by_type[Types::PAWN] | pieces_by_type[Types::KNIGHT] | pieces_by_type[Types::BISHOP] | pieces_by_type[Types::ROOK] | pieces_by_type[Types::QUEEN] | pieces_by_type[Types::KING])).get_u64());

	// Pieces by color should not intersect with each other
	assert((pieces_by_color[Colors::WHITE] & pieces_by_color[Colors::BLACK]).get_u64() == 0ULL);

	// Pieces of different types should not intersect with each other
	assert(((pieces_by_type[Types::PAWN]   & pieces_by_type[Types::KNIGHT])).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::PAWN]   & pieces_by_type[Types::BISHOP])).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::PAWN]   & pieces_by_type[Types::ROOK]  )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::PAWN]   & pieces_by_type[Types::QUEEN] )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::PAWN]   & pieces_by_type[Types::KING]  )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::KNIGHT] & pieces_by_type[Types::BISHOP])).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::KNIGHT] & pieces_by_type[Types::ROOK]  )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::KNIGHT] & pieces_by_type[Types::QUEEN] )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::KNIGHT] & pieces_by_type[Types::KING]  )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::BISHOP] & pieces_by_type[Types::ROOK]  )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::BISHOP] & pieces_by_type[Types::QUEEN] )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::BISHOP] & pieces_by_type[Types::KING]  )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::ROOK]   & pieces_by_type[Types::QUEEN] )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::ROOK]   & pieces_by_type[Types::KING]  )).get_u64() == 0ULL);
	assert(((pieces_by_type[Types::QUEEN]  & pieces_by_type[Types::KING]  )).get_u64() == 0ULL);

	// Pawns should never be on 1st or 8th rank
	Bitboard first_and_eighth  = Bitboard(0xff000000000000ffULL);
	assert(((pieces_by_type[Types::PAWN] & first_and_eighth)).get_u64() == 0ULL);

	// Never more than 8 pawns of each color
	assert((pieces_by_color[Colors::WHITE] & pieces_by_type[Types::PAWN]).popcount() <= 8);
	assert((pieces_by_color[Colors::BLACK] & pieces_by_type[Types::PAWN]).popcount() <= 8);

	// Never more than 16 pieces of each color
	assert(pieces_by_color[Colors::WHITE].popcount() <= 16);
	assert(pieces_by_color[Colors::BLACK].popcount() <= 16);

	// There must always be exactly 1 king of each color
	assert((pieces_by_color[Colors::WHITE] & pieces_by_type[Types::KING]).popcount() == 1);
	assert((pieces_by_color[Colors::BLACK] & pieces_by_type[Types::KING]).popcount() == 1);

	// The e.p. square must be on the 3rd or 6th rank (depending on whose turn)
	uint64_t third = 0x0000ff0000000000ULL;
	uint64_t sixth = 0x0000000000ff0000ULL;

	if (!epsq.is_empty()) {
		// The epsq must be a single square
		ASSERT_ONE_SQUARE(epsq);
		if (get_turn() == Colors::WHITE) {
			assert((epsq.get_u64() & sixth) == epsq.get_u64());
		}
		else if (get_turn() == Colors::BLACK) {
			assert((epsq.get_u64() & third) == epsq.get_u64());
		}
		else {
			assert(false);
		}
	}


	// ply_clock must be no greater than ply
	assert(ply_clock <= ply);

	return;
}

Position& Position::mirror_position() {

	// Flip the pieces by color bitboards
	for (auto& bb : pieces_by_color) {
		bb.mirror_v();
	}

	// Flip the pieces by type bitboards
	for (auto& bb : pieces_by_type) {
		bb.mirror_v();
	}

	// swap the colors around
	std::swap(pieces_by_color[Colors::WHITE], pieces_by_color[Colors::BLACK]);

	// swap castle rights
	uint64_t new_flags =
		((flags & 0b00000011) << 2) |
		((flags & 0b00001100) >> 2);

	flags = ((flags & 0b11110000) | new_flags);

	// flip the epsq to the other side of the board
	if (get_turn() == Colors::WHITE) {
		epsq = epsq >> 24;
	}
	else if (get_turn() == Colors::BLACK) {
		epsq = epsq << 24;
	}
	else {
		assert(false);
	}

	// increment the ply to make it the opponents turn
	++ply;
	check_integrity();
	return *this;
}

Colors Position::get_turn() {
	return static_cast<Colors>(((ply - 1) % 2));
}

Types Position::get_type(Square sq) {
	for (int i = 0; i < pieces_by_type.size(); ++i) {
		if (pieces_by_type[i].contains(sq)) { return static_cast<Types>(i); }
	}
	return Types::NONE;
}

Square Position::get_king_square(Colors color) {
	Square king_sq;
	king_sq = pieces_by_type[Types::KING] & pieces_by_color[color];
	assert(pieces_by_color[color].contains(king_sq));
	assert(pieces_by_type[Types::KING].contains(king_sq));
	return king_sq;
};

bool Position::is_type(Square sq, Types type) {
	return pieces_by_type[type].contains(sq);
}

bool Position::is_open(Square sq)
{
	return ((pieces_by_color[Colors::WHITE] | pieces_by_color[Colors::BLACK]) & sq).is_empty();
}

void Position::set_in_check(bool set) {
	if (set) {
		flags |= (1 << 4); // set the "in-check" bit.
	}
	else {
		flags &= (~(1 << 4)); // unset the "in-check" bit.
	}
}

bool Position::is_in_check() {
	return flags & (1 << 4);
}

bool Position::is_capture_move(Move move) {
	return is_opponent(move.get_to());
}

bool Position::is_opponent(Square sq) {
	return pieces_by_color[!get_turn()].contains(sq);
}

bool Position::is_friend(Square sq) {
	return pieces_by_color[get_turn()].contains(sq);
}

std::vector<Square> Position::find_attackers(Square sq, Types piece_type, Colors attacker) {
	Ray search_path = Ray(sq);
	Square to;
	int max_distance;
	std::vector<Directions> directions;
	std::vector<Square> attackers;

	if (piece_type == Types::PAWN || piece_type == Types::KNIGHT || piece_type == Types::KING) {
		max_distance = 1;

		if (piece_type == Types::PAWN) {
			// the pawn capture directions need to be from the perspective of the defending player, since we are searching for 
			// attackers from the perspective of the square that is under attack.
			// Another way to think about it:
			// If I placed one of my pawns on this square, could it capture a pawn? if so, then that other pawn
			// could also capture me, which means it attacks this square.
			directions = pawn_capt_directions[!attacker];
		}
		else {
			directions = move_directions[piece_type];
		}
	}
	else {
		directions = move_directions[piece_type];
		max_distance = NO_MAX;
	}

	for (auto dir : directions) {
		search_path.reset(dir, max_distance);
		while (!search_path.end()) {
			to = (++search_path).get_current();
			if (pieces_by_color[attacker].contains(to)) {
				if (is_type(to, piece_type)) { // found an opponent's piece of the correct type
					attackers.push_back(to);
					break;
				}
				// enemy piece was found, but not of the correct type, so we should stop the search.
				break;
			}
			else if (pieces_by_color[!attacker].contains(to)) { // found a friendly piece, which blocks enemy pieces!
				break;
			}
			// otherwise continue the search
		}
	}
	return attackers;
}

bool Position::square_covered(Square sq, Colors attacker) {
	for (int i = 0; i < 6; ++i) {
		if (!find_attackers(sq, static_cast<Types>(i), attacker).empty()) {
			return true;
		}
	}
	return false;
}

void Position::king_threats() {
	auto king_sq = (pieces_by_type[Types::KING] & pieces_by_color[get_turn()]).pop_occupied();
	assert(!king_sq.is_empty());

	Ray search_path = Ray(king_sq);
	Square to, temp_pin;
	Bitboard temp_attack_vector;
	int max_distance;
	std::vector<Directions> directions;

	// Reset all threat bitboards / vectors of bitboards
	checking_pieces.clear();
	spying_pieces.clear();
	pinned_pieces.clear();
	check_vectors.clear();
	spy_vectors.clear();

	for (int i = 0; i < 6; ++i) {
		auto piece_type = static_cast<Types>(i);
		switch (piece_type) {
		case Types::PAWN:
			max_distance = 1;
			directions = pawn_capt_directions[get_turn()];
			break;
		case Types::KNIGHT:
			max_distance = 1;
			directions = move_directions[piece_type];
			break;
		case Types::KING:
			continue;
		default:
			max_distance = NO_MAX;
			directions = move_directions[piece_type];
			break;
		}

		for (auto& dir : directions) {
			// reinit data structures
			search_path.reset(dir, max_distance);
			temp_pin = Square();
			temp_attack_vector = Bitboard();
			while (!search_path.end()) {
				to = (++search_path).get_current();
				temp_attack_vector.mark_square(to);
				if (pieces_by_color[get_turn()].contains(to)) {
					if (temp_pin.is_empty()) {
						temp_pin = to;
					}
					else {
						break;
					}
				}
				else if (pieces_by_color[!get_turn()].contains(to)) {
					if (pieces_by_type[piece_type].contains(to)) {
						if (temp_pin.is_empty()) {
							checking_pieces.mark_square(to);
							check_vectors.push_back(temp_attack_vector);
						}
						else {
							spying_pieces.mark_square(to);
							pinned_pieces.mark_square(temp_pin);
							spy_vectors.push_back(temp_attack_vector);
						}
					}
					break;
				}
				else {
					temp_attack_vector.mark_square(to);
				}
			}

		}
	}
	set_in_check(checking_pieces.popcount() > 0);
}

std::vector<Move> Position::move_gen_generic(Square from, std::vector<Directions> directions, int max_distance, MoveOptions move_opts) {
	Ray search_path = Ray(from);
	Square to;
	std::vector<Move> moves;
	Types from_type = get_type(from);

	assert(from_type != NONE); // make sure that the piece we are generating moves for isnt a none type
	assert(pieces_by_type[from_type].contains(from)); // ensure that from is a piece of the correct type

	for (auto dir : directions) {	// loop over each given direction.
		assert(dir != 0);			// cannot allow a no-direction.
		search_path.reset(dir, max_distance);		// reset the search path to the from square with the current direction and distance.

		while (!search_path.end()) { // keep incrementing in the specified direction until the end of the ray
			to = (++search_path).get_current(); // increment the ray and return that new square

			// the condition evaluates if the "to" square intersects with its own pieces. if so, this move is illegal and no more moves in this direction are legal.
			if (pieces_by_color[get_turn()].contains(to)) {
				assert(get_type(to) != NONE);
				if (move_opts == MoveOptions::SELF_CAPT) {
					moves.push_back(Move(from, to, get_turn(), from_type, get_type(to)));
				}
				break;
			}

			// the condition evaluates if the "to" square intersects with the opponents pieces. if so, this move is a capture and no more moves in this direction are legal.
			if (pieces_by_color[!get_turn()].contains(to)) {
				assert(get_type(to) != NONE); // make sure that we arent capturing a none type
				if (move_opts == MoveOptions::CAPT) {
					moves.push_back(Move(from, to, get_turn(), from_type, get_type(to)));
				}
				break;
			}

			// if neither previous condition evaluates true, then this move is legal and we can continue in this direction.
			if (move_opts == MoveOptions::PLACE) {
				moves.push_back(Move(from, to, get_turn(), from_type));
			}
		}
	}
	return moves;
}

std::vector<Move> Position::move_gen_sliders(Square from, Types type) {
	assert(type != PAWN && type != KING); // function cannot be used with pawns or kings.
	Bitboard attack_vector = Bitboard();
	Square test_sq;
	int test_dir, index;
	std::vector<Directions> dirs, piece_dirs;
	std::vector<Move> moves, temp_moves;

	int max_distance = -1;
	if (type == KNIGHT) { max_distance = 1; };

	if (!is_in_check()) { // if the king is not in check...
		if (pinned_pieces.contains(from)) { // if this piece is pinned...
			for (index = 0; index < spy_vectors.size(); ++index) { // loop across all the spy vectors...
				if (spy_vectors[index].contains(from)) { // if this spy vector contains the square that this piece is on...
					attack_vector = spy_vectors[index]; // then we have found the corresponding attack vector.
					break; 
				}
			}

			assert(!attack_vector.is_empty()); // The attack vector cannot be empty if this piece is pinned.

			piece_dirs = move_directions[type]; // Start with the full set of move directions for this piece.
			while (!attack_vector.is_empty()) { // while the attack vector is not empty...
				test_sq = attack_vector.pop_occupied(); // pop the next occupied square
				test_dir = test_sq.direction_from(from); // compute the basic direction from the pinned piece to the square being tested.
				auto matched_dir = std::find(piece_dirs.begin(), piece_dirs.end(), test_dir); // search the move directions of this piece for the computed direction
				if (matched_dir != piece_dirs.end()) { // if the computed direction is one of the possible move directions...
					dirs.push_back(*matched_dir); // add this direction to a list of directions.
				}
			}

			temp_moves = move_gen_generic(from, dirs, max_distance); // Compute the psuedolegal moves with the subset of directions found.
			for (auto& temp_move : temp_moves) { // for each move...
				test_sq = temp_move.get_to(); // get the square being moved to
				if (spy_vectors[index].contains(test_sq)) { // and if the move is still in the same spy vector...
					moves.push_back(temp_move); // the move is valid, so we add it to the list.
				}
			}
		}
		else { // if the piece is not pinned...
			moves = move_gen_generic(from, move_directions[type], max_distance); // all of its potential moves are legal, so compute them all.
		}
	}
	else { // the king is in check...
		assert(checking_pieces.popcount() == 1); // the king must only be in single check, outer function ensures this should be the case but this code is not valid for multiple check.

		if (pinned_pieces.contains(from)) { // if this piece is pinned...
			moves = {}; // there are no legal moves it can make.
		}
		else { // if the piece is not pinned...
			attack_vector = check_vectors[0]; // get the attack vector of the checking piece
			while (!attack_vector.is_empty()) { // while the attack vector is not empty...
				test_sq = attack_vector.pop_occupied(); // get the next occupied square
				test_dir = test_sq.direction_from(from); // compute the direction from this piece to the test square.
				auto matched_dir = std::find(piece_dirs.begin(), piece_dirs.end(), test_dir); // search this pieces directions for the computed direction
				if (matched_dir != piece_dirs.end()) { // if the direction was found...
					dirs.push_back(*matched_dir); // add this direction to the possible directions to search.
				}
			}
			temp_moves = move_gen_generic(from, dirs, max_distance); // get the subset of moves in the matching directions
			for (auto temp_move : temp_moves) { // for each move...
				auto temp_sq = temp_move.get_to(); // get the square the move is going to
				if (attack_vector.contains(temp_sq)) { moves.push_back(temp_move); } // and if it is in the attack vector, add to the valid moves.
			}
		}
	}
	return moves;
}

std::vector<Move> Position::move_gen_k(Square from)
{
	assert(pieces_by_type[Types::KING].contains(from)); //ensure that this piece is a king
	std::vector<Move> moves, temp_moves;

	temp_moves = move_gen_generic(from, move_directions[Types::KING], 1);	// Get king's psuedolegal moves
	for (auto& temp_move : temp_moves) {							// for each psuedolegal move...
		if (!square_covered(temp_move.get_to(), !get_turn())) {					// if the move is to an unattacked square...
			moves.push_back(temp_move);								// add it to the legal move list
		}
	}

	if (!is_in_check()) { // if not in check...
		if (K_castling_right() && is_castling_legal(CastleSide::KINGSIDE)) { // see if we have the right to castle kingside
			temp_moves = move_gen_generic(from, { Directions::KING_KINGSIDE_CASTLE }, 1); 
			assert(temp_moves.size() == 1);

			// update move to be the more specific move type of castling
			temp_moves[0].set_move_type(SpecialMoves::CASTLE).set_special(from << 3); // changes to a castle move and adds the King's rook square to the special field
			moves.push_back(temp_moves[0]);
		}
		if (Q_castling_right() && is_castling_legal(CastleSide::QUEENSIDE)) { // see if we have the right to castle queenside
			temp_moves = move_gen_generic(from, { Directions::KING_QUEENSIDE_CASTLE }, 1);
			assert(temp_moves.size() == 1);

			temp_moves[0].set_move_type(SpecialMoves::CASTLE).set_special(from >> 4); // changes to a castle move and adds the Queen's rook square to the special field
			moves.push_back(temp_moves[0]);
		}
	}
	return moves;
}

std::vector<Move> Position::move_gen_p(Square from) {
	ASSERT_ONE_SQUARE(from);
	std::vector<Move> pl_moves, moves, temp_moves;
	Bitboard attack_vector;
	unsigned int max_distance;
	Colors turn = get_turn();

	// Start by calculating 
	if (is_in_check() && pinned_pieces.contains(from)) { return {}; } // dont calculate any moves if the king is both in check and this piece is pinned (cuz it isnt possible to move)

	// Get pawn push moves
	(from.on_pawn_start_rank(turn)) ? (max_distance = 2) : (max_distance = 1);
	pl_moves = move_gen_generic(from, { move_directions[Types::PAWN][turn] }, max_distance, MoveOptions::PLACE);

	// Get standard pawn capture moves
	temp_moves = move_gen_generic(from, pawn_capt_directions[turn], 1, MoveOptions::CAPT);
	pl_moves.insert(pl_moves.end(), temp_moves.begin(), temp_moves.end());

	// Check all generated moves to see if any are eligible for promotion
	int old_size = pl_moves.size();
	for (int i = 0; i < old_size; ++i) {
		if (pl_moves[i].get_to().on_promote_rank(turn)) {
			Move promote_move = pl_moves[i].set_move_type(SpecialMoves::PROMOTION);
			for (int j = 1; j < 5; ++j) {
				promote_move.set_promote_type(static_cast<Types>(j));
				if (j == 1) {
					pl_moves[i] = promote_move; // overwrite the found move with the first promotion move
				}
				else {
					pl_moves.push_back(promote_move); // append the remaining promotion moves
				}
			}
		}
	}

	// En Passant moves
	if (!epsq.is_empty()) {
		if (turn == WHITE) {
			if ((from >> 7) == epsq && !from.on_nth_file(7)) {
				pl_moves.push_back(Move(from, epsq, from << 1, turn, PAWN, PAWN, NONE, SpecialMoves::EN_PASSANT));
			}
			if ((from >> 9) == epsq && !from.on_nth_file(0)) {
				pl_moves.push_back(Move(from, epsq, from >> 1, turn, PAWN, PAWN, NONE, SpecialMoves::EN_PASSANT));
			}
		}
		else {
			if ((from << 7) == epsq && !from.on_nth_file(0)) {
				pl_moves.push_back(Move(from, epsq, from >> 1, turn, PAWN, PAWN, NONE, SpecialMoves::EN_PASSANT));
			}
			if ((from << 9) == epsq && !from.on_nth_file(7)) {
				pl_moves.push_back(Move(from, epsq, from << 1, turn, PAWN, PAWN, NONE, SpecialMoves::EN_PASSANT));
			}
		}
	}

	if (is_in_check()) {
		assert(check_vectors.size() == 1); // Only in single check
		attack_vector = check_vectors[0];
	}
	else if (pinned_pieces.contains(from)) {
		for (auto& test_vector : spy_vectors) {
			if (test_vector.contains(from)) {
				attack_vector = test_vector;
				break;
			}
		}
		for (auto& pl_move : pl_moves) {
			auto test_sq = pl_move.get_to();
			if (attack_vector.contains(test_sq)) {
				moves.push_back(pl_move);
			}
		}
	}
	else {
		moves = pl_moves;
	}
	return moves;
}

std::vector<Move> Position::move_gen() {
	Colors turn = get_turn();
	Square from;
	Bitboard pieces;
	Types type;
	std::vector<Move> moves, temp_moves;

	if (checking_pieces.popcount() > 1) { // if in double or more (!!) check...
		moves = move_gen_k(pieces_by_type[KING] & pieces_by_color[get_turn()]); // Only have to search for King moves
	}
	else {
		for (int i = 0; i < pieces_by_type.size(); ++i) { // for each piece type...
			pieces = pieces_by_type[i] & pieces_by_color[turn]; // get the pieces of that type and of the right color
			type = static_cast<Types>(i); // calculate the type from the iteration var

			while (!pieces.is_empty()) { // while there are pieces left on the board...
				from = pieces.pop_occupied(); // remove the next occupied square from the set of pieces
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
	}
	return moves;
}

std::vector<Move> Position::BASIC_pl_move_gen() {
	Bitboard pieces = pieces_by_color[get_turn()];
	Square cur_piece;
	Types type;
	std::vector<Move> moves = {}, temp_moves;

	while (!pieces.is_empty()) {
		cur_piece = pieces.pop_occupied();
		type = get_type(cur_piece);
		assert(type != Types::NONE);

		if (type == Types::PAWN) { // cant call the std_move_gen with pawns
			temp_moves = BASIC_pl_pawn_move_gen(cur_piece);
			moves.insert(moves.end(), temp_moves.begin(), temp_moves.end());
		}
		else {
			temp_moves = BASIC_pl_std_move_gen(cur_piece);
			moves.insert(moves.end(), temp_moves.begin(), temp_moves.end());
		}

		if (type == Types::KING) { // castling moves are in addition to all other std moves
			temp_moves = BASIC_pl_castle_move_gen(cur_piece);
			moves.insert(moves.end(), temp_moves.begin(), temp_moves.end());
		}
	}
	return moves;
}

std::vector<Move> Position::BASIC_pl_std_move_gen(Square from) {
	std::vector<Move> moves = {};
	Types type = get_type(from);
	Square to;
	Ray search = Ray(from);
	auto dirs = move_directions[type];
	int max_distance;
	if (type == KNIGHT || type == KING) { max_distance = 1; }
	else { max_distance = NO_MAX; }

	assert(type != Types::PAWN); // Pawns will not work with this code.
	assert(type != Types::NONE); // must select a piece
	
	for (auto dir : dirs) {
		search.reset(dir, max_distance);
		while (!search.end()) {
			++search;
			to = search.get_current();

			// determine what is on the test square and do something about it.
			if (is_open(to)) {
				moves.push_back(Move(from, to, get_turn(), type));
			}
			else {
				if (is_opponent(to)) {
					if (to != get_king_square(!get_turn())) {
						moves.push_back(Move(from, to, get_turn(), type, get_type(to)));
					}
					break; // searching this ray is done
				}
				else if (is_friend(to)) {
					break; // searching this ray is done
				}
				else { // if the square is not open, not a friend, nor an opponent, then something has gone wrong.
					assert(false);
				}
			}
			
		}
	}
	
	return moves;
}

std::vector<Move> Position::BASIC_pl_pawn_move_gen(Square from) {
	std::vector<Move> moves = {};
	Types type = get_type(from);
	Square to, special;
	Ray search = Ray(from);
	int max_distance;
	Colors turn = get_turn();
	auto dir = move_directions[type][turn];
	auto capt_dirs = pawn_capt_directions[turn];

	assert(type != Types::NONE); // Must select a piece
	assert(type == PAWN); // Only pawns will work with this code.

	// This lambda takes the calculated move and determines if it is eligible for promotion. 
	// It then decides to either store the set of promotions or just the plain move.
	auto save_moves = [&](Move move) -> void {
		if ((turn == Colors::WHITE && to.on_nth_rank(7)) || (turn == Colors::BLACK && to.on_nth_rank(0))) {
			for (int i = 1; i < 5; ++i) {
				auto promote_move = move;
				promote_move.set_move_type(SpecialMoves::PROMOTION);
				promote_move.set_promote_type(static_cast<Types> (i));
				moves.push_back(promote_move);
			}
		}
		else {
			moves.push_back(move);
		}
	};

	// check pawn push moves
	if (turn == Colors::WHITE && from.on_nth_rank(1)) { max_distance = 2; }
	else if (turn == Colors::BLACK && from.on_nth_rank(6)) { max_distance = 2; }
	else { max_distance = 1; }
	search.reset(dir, max_distance);
	while (!search.end()) {
		++search;
		to = search.get_current();

		// determine what is on the test square and do something about it.
		if (is_open(to)) {
			Move move = Move(from, to, turn, type);
			save_moves(move);
		}
		else {
			break; // occupied squares are not available for pawn pushes
		}
	}

	// check pawn capture moves
	for (auto dir : capt_dirs) {
		search.reset(dir, 1);
		if (!search.end()) {
			++search;
			to = search.get_current();

			if (is_opponent(to)) { // std capture move
				if (to != get_king_square(!turn)) { // excluding king captures
					Move move = Move(from, to, turn, type, get_type(to));
					save_moves(move);
				}
			}
			else if (to == epsq) { // e.p. capture move
				if (turn == Colors::WHITE) { special = to << 8; }
				else if (turn == Colors::BLACK) { special = to >> 8; }
				else { assert(false); }
				Move move = Move(from, to, special, turn, type, Types::PAWN, Types::NONE, SpecialMoves::EN_PASSANT);
				moves.push_back(move); // Dont need to call save_moves() here since an en passant could never be a promotion.
			}
		}
	}
	return moves;
}

std::vector<Move> Position::BASIC_pl_castle_move_gen(Square from) {
	
	if (is_in_check()) {
		return std::vector<Move>();
	}

	std::vector<Move> moves;
	Colors turn = get_turn();
	Bitboard between_q, between_k;
	Square sq_k;
	bool q_right, k_right;

	if (turn == Colors::BLACK) {
		between_q = Bitboard(0x000000000000000e);
		between_k = Bitboard(0x0000000000000060);
		sq_k      = Square  (0x0000000000000010ULL); // ULL needed to avoid calling the Square(int sq) constructor
	}
	else {
		between_q = Bitboard(0x0e00000000000000);
		between_k = Bitboard(0x6000000000000000);
		sq_k      = Square  (0x1000000000000000ULL); // ULL needed to avoid calling the Square(int sq) constructor
	}

	// Check queenside first
	if (Q_castling_right()) { // do we have the right to castle?
		if ((get_occupied() & between_q).is_empty()) { // are the squares between the king and rook empty?
			if (!square_covered(sq_k >> 1, !turn) && !square_covered(sq_k >> 2, !turn)) { // are the squares the king moves through attacked?
				Move move = Move(sq_k, sq_k >> 2, sq_k >> 4, turn, Types::KING, Types::NONE, Types::NONE, SpecialMoves::CASTLE);
				moves.push_back(move);
			}
		}
	}

	// Check kingside second
	if (K_castling_right()) { // do we have the right to castle?
		if ((get_occupied() & between_k).is_empty()) { // are the squares between the king and rook empty?
			if (!square_covered(sq_k << 1, !turn) && !square_covered(sq_k << 2, !turn)) { // are the squares the king moves through attacked?
				Move move = Move(sq_k, sq_k << 2, sq_k << 3, turn, Types::KING, Types::NONE, Types::NONE, SpecialMoves::CASTLE);
				moves.push_back(move);
			}
		}
	}
	return moves;
}

std::vector<Move> Position::BASIC_move_gen() {
	std::vector<Move> moves;
	Colors turn = get_turn();

	moves = BASIC_pl_move_gen(); // Get all the pseudo-legal moves in the position.

	// lambda function for filtering illegal moves.
	auto illegal_move = [&](Move& pl_move) -> bool {
		Position next = Position(*this);
		next.make_move(pl_move);
		return next.square_covered(next.get_king_square(turn), !turn);
	};


	moves.erase(std::remove_if(moves.begin(), moves.end(), illegal_move), moves.end());

	return moves;
}

void Position::BASIC_king_threats() {
	// HAS to come before the ply increment, otherwise get_turn() will get the wrong color and the opponents king.
	Square king_sq = pieces_by_color[get_turn()] & pieces_by_type[Types::KING];
	set_in_check(square_covered(king_sq, !get_turn()));
	return;
}

bool Position::is_position_illegal()
{
	Square king_sq = pieces_by_color[!get_turn()] & pieces_by_type[Types::KING];
	
	std::vector<Move> attacks = BASIC_move_gen();

	// check each move for the opponent to see if they "capture" the king
	for (auto& attack : attacks) {
		if (attack.get_to() == king_sq) {
			return true;
		}
	}
	return false;
}

void MoveIntegrityChecker::get_before_stats(Position pos, Colors turn) {
	friendly_piece_count_before = pos.pieces_by_color[turn].popcount();
	enemy_piece_count_before = pos.pieces_by_color[!turn].popcount();
	rook_piece_count_before = pos.pieces_by_type[Types::ROOK].popcount();
	knight_piece_count_before = pos.pieces_by_type[Types::KNIGHT].popcount();
	queen_piece_count_before = pos.pieces_by_type[Types::QUEEN].popcount();
	king_piece_count_before = pos.pieces_by_type[Types::KING].popcount();
	pawn_piece_count_before = pos.pieces_by_type[Types::PAWN].popcount();
	bishop_piece_count_before = pos.pieces_by_type[Types::BISHOP].popcount();
	covered_squares_before = pos.get_occupied().get_u64();
}

void MoveIntegrityChecker::get_after_stats(Position pos, Colors turn) {
	friendly_piece_count_after = pos.pieces_by_color[turn].popcount();
	enemy_piece_count_after = pos.pieces_by_color[!turn].popcount();
	rook_piece_count_after = pos.pieces_by_type[Types::ROOK].popcount();
	knight_piece_count_after = pos.pieces_by_type[Types::KNIGHT].popcount();
	queen_piece_count_after = pos.pieces_by_type[Types::QUEEN].popcount();
	king_piece_count_after = pos.pieces_by_type[Types::KING].popcount();
	pawn_piece_count_after = pos.pieces_by_type[Types::PAWN].popcount();
	bishop_piece_count_after = pos.pieces_by_type[Types::BISHOP].popcount();
	covered_squares_after = pos.get_occupied().get_u64();
}

void MoveIntegrityChecker::check_move_integrity(Position pos, Colors turn, Move move) {
	get_after_stats(pos, turn);
	Types capt_type = move.get_capt_type();
	Types promo_type = move.get_promote_type();
	SpecialMoves move_type = move.get_move_type();
	uint64_t special = move.get_special().get_u64();
	uint64_t to = move.get_to().get_u64();
	uint64_t from = move.get_from().get_u64();
	bool capture_move = (capt_type != Types::NONE);

	assert(friendly_piece_count_before == friendly_piece_count_after);
	assert(king_piece_count_before == king_piece_count_after);

	// checking piece counts
	switch (move_type) {
	case SpecialMoves::STD:
		if (capture_move) {
			assert(enemy_piece_count_before == enemy_piece_count_after + 1);
			switch (capt_type) {
			case Types::PAWN:
				assert(pawn_piece_count_before == pawn_piece_count_after + 1);
				assert(knight_piece_count_before == knight_piece_count_after);
				assert(bishop_piece_count_before == bishop_piece_count_after);
				assert(rook_piece_count_before == rook_piece_count_after);
				assert(queen_piece_count_before == queen_piece_count_after);
				break;
			case Types::KNIGHT:
				assert(pawn_piece_count_before == pawn_piece_count_after);
				assert(knight_piece_count_before == knight_piece_count_after + 1);
				assert(bishop_piece_count_before == bishop_piece_count_after);
				assert(rook_piece_count_before == rook_piece_count_after);
				assert(queen_piece_count_before == queen_piece_count_after);
				break;
			case Types::BISHOP:
				assert(pawn_piece_count_before == pawn_piece_count_after);
				assert(knight_piece_count_before == knight_piece_count_after);
				assert(bishop_piece_count_before == bishop_piece_count_after + 1);
				assert(rook_piece_count_before == rook_piece_count_after);
				assert(queen_piece_count_before == queen_piece_count_after);
				break;
			case Types::ROOK:
				assert(pawn_piece_count_before == pawn_piece_count_after);
				assert(knight_piece_count_before == knight_piece_count_after);
				assert(bishop_piece_count_before == bishop_piece_count_after);
				assert(rook_piece_count_before == rook_piece_count_after + 1);
				assert(queen_piece_count_before == queen_piece_count_after);
				break;
			case Types::QUEEN:
				assert(pawn_piece_count_before == pawn_piece_count_after);
				assert(knight_piece_count_before == knight_piece_count_after);
				assert(bishop_piece_count_before == bishop_piece_count_after);
				assert(rook_piece_count_before == rook_piece_count_after);
				assert(queen_piece_count_before == queen_piece_count_after + 1);
				break;
			}
		}
		else {
			assert(enemy_piece_count_before == enemy_piece_count_after);
			assert(pawn_piece_count_before == pawn_piece_count_after);
			assert(knight_piece_count_before == knight_piece_count_after);
			assert(bishop_piece_count_before == bishop_piece_count_after);
			assert(rook_piece_count_before == rook_piece_count_after);
			assert(queen_piece_count_before == queen_piece_count_after);
		}
		break;
	case SpecialMoves::EN_PASSANT:
		assert(enemy_piece_count_before == enemy_piece_count_after + 1);
		assert(pawn_piece_count_before == pawn_piece_count_after + 1);
		assert(knight_piece_count_before == knight_piece_count_after);
		assert(bishop_piece_count_before == bishop_piece_count_after);
		assert(rook_piece_count_before == rook_piece_count_after);
		assert(queen_piece_count_before == queen_piece_count_after);
		break;

	case SpecialMoves::CASTLE:
		assert(enemy_piece_count_before == enemy_piece_count_after);
		assert(pawn_piece_count_before == pawn_piece_count_after);
		assert(knight_piece_count_before == knight_piece_count_after);
		assert(bishop_piece_count_before == bishop_piece_count_after);
		assert(rook_piece_count_before == rook_piece_count_after);
		assert(queen_piece_count_before == queen_piece_count_after);
		break;

	case SpecialMoves::PROMOTION:
		assert(capt_type != Types::KING && capt_type != Types::PAWN);
		assert(pawn_piece_count_before == pawn_piece_count_after + 1);

		if (capture_move) {
			assert(enemy_piece_count_before == enemy_piece_count_after + 1);
			switch (promo_type) {
			case Types::KNIGHT:
				switch (capt_type) {
				case Types::KNIGHT:
					assert(knight_piece_count_before == knight_piece_count_after - 1 + 1);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::BISHOP:
					assert(knight_piece_count_before == knight_piece_count_after - 1);
					assert(bishop_piece_count_before == bishop_piece_count_after + 1);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::ROOK:
					assert(knight_piece_count_before == knight_piece_count_after - 1);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after + 1);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::QUEEN:
					assert(knight_piece_count_before == knight_piece_count_after - 1);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after + 1);
					break;
				}
				break;
			case Types::BISHOP:
				switch (capt_type) {
				case Types::KNIGHT:
					assert(knight_piece_count_before == knight_piece_count_after + 1);
					assert(bishop_piece_count_before == bishop_piece_count_after - 1);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::BISHOP:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after - 1 + 1);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::ROOK:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after - 1);
					assert(rook_piece_count_before == rook_piece_count_after + 1);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::QUEEN:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after - 1);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after + 1);
					break;
				}
				break;
			case Types::ROOK:
				switch (capt_type) {
				case Types::KNIGHT:
					assert(knight_piece_count_before == knight_piece_count_after + 1);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after - 1);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::BISHOP:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after + 1);
					assert(rook_piece_count_before == rook_piece_count_after - 1);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::ROOK:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after - 1 + 1);
					assert(queen_piece_count_before == queen_piece_count_after);
					break;
				case Types::QUEEN:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after - 1);
					assert(queen_piece_count_before == queen_piece_count_after + 1);
					break;
				}
				break;
			case Types::QUEEN:
				switch (capt_type) {
				case Types::KNIGHT:
					assert(knight_piece_count_before == knight_piece_count_after + 1);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after - 1);
					break;
				case Types::BISHOP:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after + 1);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after - 1);
					break;
				case Types::ROOK:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after + 1);
					assert(queen_piece_count_before == queen_piece_count_after - 1);
					break;
				case Types::QUEEN:
					assert(knight_piece_count_before == knight_piece_count_after);
					assert(bishop_piece_count_before == bishop_piece_count_after);
					assert(rook_piece_count_before == rook_piece_count_after);
					assert(queen_piece_count_before == queen_piece_count_after - 1 + 1);
					break;
				}
				break;
			}
		}
		else {
			assert(enemy_piece_count_before == enemy_piece_count_after);

			switch (promo_type) {
			case Types::KNIGHT:
				assert(knight_piece_count_before == knight_piece_count_after - 1);
				assert(bishop_piece_count_before == bishop_piece_count_after);
				assert(rook_piece_count_before == rook_piece_count_after);
				assert(queen_piece_count_before == queen_piece_count_after);
				break;
			case Types::BISHOP:
				assert(knight_piece_count_before == knight_piece_count_after);
				assert(bishop_piece_count_before == bishop_piece_count_after - 1);
				assert(rook_piece_count_before == rook_piece_count_after);
				assert(queen_piece_count_before == queen_piece_count_after);
				break;
			case Types::ROOK:
				assert(knight_piece_count_before == knight_piece_count_after);
				assert(bishop_piece_count_before == bishop_piece_count_after);
				assert(rook_piece_count_before == rook_piece_count_after - 1);
				assert(queen_piece_count_before == queen_piece_count_after);
				break;
			case Types::QUEEN:
				assert(knight_piece_count_before == knight_piece_count_after);
				assert(bishop_piece_count_before == bishop_piece_count_after);
				assert(rook_piece_count_before == rook_piece_count_after);
				assert(queen_piece_count_before == queen_piece_count_after - 1);
				break;
			}
		}
		break;
	}

	// checking piece coverage
	switch (move_type) {
	case SpecialMoves::EN_PASSANT:
		assert((covered_squares_before ^ from ^ special ^ to) == covered_squares_after);
		break;
	case SpecialMoves::CASTLE:
		if (turn == Colors::WHITE) {
			if (special == WHITE_QUEENSIDE_ROOK_STARTING_SQUARE.get_u64()) {
				assert((covered_squares_before ^ from ^ to ^ special ^ (from >> 1)) == covered_squares_after);
			}
			else if (special == WHITE_KINGSIDE_ROOK_STARTING_SQUARE.get_u64()) {
				assert((covered_squares_before ^ from ^ to ^ special ^ (from << 1)) == covered_squares_after);
			}
		}
		else if (turn == Colors::BLACK) {
			if (special == BLACK_QUEENSIDE_ROOK_STARTING_SQUARE.get_u64()) {
				assert((covered_squares_before ^ from ^ to ^ special ^ (from >> 1)) == covered_squares_after);

			}
			else if (special == BLACK_KINGSIDE_ROOK_STARTING_SQUARE.get_u64()) {
				assert((covered_squares_before ^ from ^ to ^ special ^ (from << 1)) == covered_squares_after);

			}
		}
		break;
	case SpecialMoves::PROMOTION:
		if (capture_move) {	assert((covered_squares_before ^ from) == covered_squares_after); }
		else { assert((covered_squares_before ^ from ^ to) == covered_squares_after); }
		break;
	case SpecialMoves::STD:
		if (capture_move) {	assert((covered_squares_before ^ from) == covered_squares_after);	}
		else { assert((covered_squares_before ^ from ^ to) == covered_squares_after); }
		break;
	}
}

void MoveIntegrityChecker::check_castling_rights(Position pos) {
	uint8_t flags = pos.flags;

	// test assumptions. For castling to be available, the king and rook must be in their starting positions.
	if (flags & WHITE_QUEENSIDE_CASTLING_RIGHTS_MASK) {
		assert(pos.pieces_by_type[Types::ROOK].contains(WHITE_QUEENSIDE_ROOK_STARTING_SQUARE));
		assert(pos.pieces_by_type[Types::KING].contains(WHITE_KING_STARTING_SQUARE));
		assert(pos.pieces_by_color[Colors::WHITE].contains(WHITE_QUEENSIDE_ROOK_STARTING_SQUARE));
		assert(pos.pieces_by_color[Colors::WHITE].contains(WHITE_KING_STARTING_SQUARE));
	}
	if (flags & WHITE_KINGSIDE_CASTLING_RIGHTS_MASK) {
		assert(pos.pieces_by_type[Types::ROOK].contains(WHITE_KINGSIDE_ROOK_STARTING_SQUARE));
		assert(pos.pieces_by_type[Types::KING].contains(WHITE_KING_STARTING_SQUARE));
		assert(pos.pieces_by_color[Colors::WHITE].contains(WHITE_KINGSIDE_ROOK_STARTING_SQUARE));
		assert(pos.pieces_by_color[Colors::WHITE].contains(WHITE_KING_STARTING_SQUARE));
	}
	if (flags & BLACK_QUEENSIDE_CASTLING_RIGHTS_MASK) {
		assert(pos.pieces_by_type[Types::ROOK].contains(BLACK_QUEENSIDE_ROOK_STARTING_SQUARE));
		assert(pos.pieces_by_type[Types::KING].contains(BLACK_KING_STARTING_SQUARE));
		assert(pos.pieces_by_color[Colors::BLACK].contains(BLACK_QUEENSIDE_ROOK_STARTING_SQUARE));
		assert(pos.pieces_by_color[Colors::BLACK].contains(BLACK_KING_STARTING_SQUARE));
	}
	if (flags & BLACK_KINGSIDE_CASTLING_RIGHTS_MASK) {
		assert(pos.pieces_by_type[Types::ROOK].contains(BLACK_KINGSIDE_ROOK_STARTING_SQUARE));
		assert(pos.pieces_by_type[Types::KING].contains(BLACK_KING_STARTING_SQUARE));
		assert(pos.pieces_by_color[Colors::BLACK].contains(BLACK_KINGSIDE_ROOK_STARTING_SQUARE));
		assert(pos.pieces_by_color[Colors::BLACK].contains(BLACK_KING_STARTING_SQUARE));
	}

}
