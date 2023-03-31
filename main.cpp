#define BASIC

// TODO: write a board display function, a bitboard display function, and a simple initialization script for the Position class to test FEN parsing.

#include <algorithm>
#include "Position.h"
#include "Colors.h"
#include "Move.h"
#include "Utils.h"
#include "Types.h"
#include "perft.h"

enum class MoveTypes {NONE = -1, CAPTURE, EN_PASSANT, N_PROMOTION, B_PROMOTION, R_PROMOTION, Q_PROMOTION, Q_CASTLE, K_CASTLE, CHECK};
enum class MoveResults {NONE = -1, CHECKMATE, STALEMATE, DISCOVERY, DOUBLE_CHECK };

int main() {
	Position current_position = Position::setup_position();
	std::vector<Position> history = { current_position }; // initialize the history
	
	bool running = true;

	// welcome message
	std::cout << "V8 Chess Engine" << std::endl << "by Colin Nordquist" << std::endl << std::endl;

	while (running) {
		std::stringstream input;
		std::string input_line, cmd, arg;
		std::vector<std::string> tokens;
		
		std::cout << "V8 > ";
		std::cin.clear();

		std::getline(std::cin, input_line);
		input.str(input_line);

		// tokenize
		while (input.good()) {
			std::string token;
			std::getline(input, token, ' ');
			if (!token.empty()) {
				tokens.push_back(token);
			}
		}
		cmd = tokens[0];

		if (cmd == "setup" || cmd == "s") {
			// a normal FEN string has lots of spaces in it, so instead of forcing the CLI user to put quotations around the FEN, 
			// this code instead just appends all the tokens together into one string before passing it to the position constructor.
			std::string arg;
			std::for_each(
				tokens.begin() + 1,
				tokens.end(),
				[&arg](std::string token) {arg.append(token).append(" ");}
			);
			arg.pop_back(); // get rid of the extra " " that was appended

			current_position = Position::setup_position(arg);
			history.clear();
			history.push_back(current_position);
		}
		else if (cmd == "display" || cmd == "d") {
			current_position.disp();
		}
		else if (cmd == "move" || cmd == "m") {
			bool valid = true;
			std::string token;

			auto fail = [&](std::string msg) {
					valid = false;
					std::cout << std::endl;
					std::cout << msg << std::endl;
					std::cout << "parsed move: \"" << token << "\"" << std::endl;
					std::cout << "moves must be in the Long Algebraic Notation format (i.e. \"a1h1\")." << std::endl << std::endl;
			};

			if (tokens.size() == 1) {
				std::cout << std::endl;
				std::cout << "no moves were given." << std::endl << std::endl;
			}

			// copy the current position in case we need to revert.
			auto start_pos = current_position;
			// create a vector for the new positions we will make.
			std::vector<Position> temp_history;

			for (auto token_it = tokens.begin() + 1; token_it != tokens.end(); token_it++) {
				token = *token_it;
				if (token.length() != 4) {
					fail("move must be exactly 4 characters long.");
					break;
				}
				auto from_sq = Square(token.substr(0, 2)); // grab the first two characters and make a square from them.
				if (from_sq.is_empty()) {
					fail("move could not be parsed to a valid set of squares.");
					break;
				}
				auto to_sq = Square(token.substr(2, 2)); // grab the last two characters and make a square from them.
				if (to_sq.is_empty()) {
					fail("move could not be parsed to a valid set of squares.");
					break;
				}

				auto legal_moves = current_position.BASIC_move_gen();
				bool move_found = false;
				for (auto legal_move : legal_moves) {
					if (legal_move.get_from() == from_sq && legal_move.get_to() == to_sq) {
						current_position.make_move(legal_move);
						temp_history.push_back(current_position);
						move_found = true;
						break; //end search
					}
				}
				if (!move_found) {
					fail("move is not valid in this position.");
					break;
				}
			}
			if (!valid) { // revert the position if the inputs were not valid.
				current_position = start_pos;
			}
			else { // otherwise add the moves to the history
				history.insert(history.end(), temp_history.begin(), temp_history.end());
			}
		}
		else if (cmd == "undo" || cmd == "u") {
			if (tokens.size() == 1) {
				history.pop_back();
				current_position = history.back();
			}
			else if (tokens.size() == 2) {
				try {
					size_t parse_len;
					int undo_len = std::stoi(tokens[1], &parse_len);
					if (parse_len != tokens[1].size()) {
						std::cout << std::endl;
						std::cout << "the \"count\" parameter was not an integer." << std::endl << std::endl;
					}
					else if (undo_len > history.size() - 1) {
						std::cout << std::endl;
						std::cout << "cannot undo more moves than have been played." << std::endl; 
						std::cout << "number of moves played: " << history.size() - 1 << std::endl << std::endl;
					}
					else {
						for (int i = 0; i < undo_len; i++) {
							history.pop_back();
						}
						current_position = history.back();
					}
				}
				catch (std::invalid_argument&) {
					std::cout << std::endl;
					std::cout << "the \"count\" parameter was not an integer." << std::endl << std::endl;
				}
				catch (std::out_of_range&) {
					std::cout << std::endl;
					std::cout << "the \"count\" parameter was too large to fit in an integer." << std::endl << std::endl;
				}
			}
			else {
				std::cout << std::endl;
				std::cout << "too many arguments were provided." << std::endl << std::endl;
			}
		}
		else if (cmd == "perft" || cmd == "p") {
			std::string depth = tokens[1];
			Perft perft = Perft(current_position, std::stoi(depth));
			perft.run();
			std::cout << std::endl << perft << std::endl;
		}
		else if (cmd == "quit" || cmd == "q") {
			running = false;
		}
		else if (cmd == "help" || cmd == "?") {
			std::cout << std::endl;
			std::cout << "Usage:" << std::endl;
			std::cout << std::endl;
			std::cout << "s[etup] <FEN>\t\t- creates a position based on the FEN string provided." << std::endl;
			std::cout << "d[isplay]\t\t- shows the current position." << std::endl;
			std::cout << "m[ove] <LAN>...\t\t- makes a move. if multiple moves are provided, will make the moves in the sequence given. moves must be in long algebraic notation." << std::endl;
			std::cout << "u[ndo] [<count>]\t- reverses a number of moves. If count is omitted, reverses 1 move." << std::endl;
			std::cout << "p[erft] <depth>\t\t- finds all moves to the depth given, and outputs aggregate information about the search." << std::endl;
			std::cout << "? | help\t\t- I think you already figured out what this does." << std::endl;
			std::cout << "q[uit]\t\t\t- exit the program." << std::endl;
			std::cout << std::endl;
		}
		else {
			std::cout << std::endl;
			std::cout << "unknown command. try \'help\' for the proper usage." << std::endl;
			std::cout << std::endl;
		}
	}
	std::cout << std::endl << "v8 X " << std::endl;
}
