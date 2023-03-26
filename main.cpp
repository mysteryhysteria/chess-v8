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
	
	bool running = true;

	// welcome message
	std::cout << "V8 Chess Engine" << std::endl << "by Colin Nordquist" << std::endl << std::endl;

	while (running) {
		std::stringstream input;
		std::string input_line, token, cmd, arg;
		std::vector<std::string> tokens;
		
		std::cout << "V8 > ";
		std::cin.clear();

		std::getline(std::cin, input_line);
		input.str(input_line);

		// tokenize
		while (input.good()) {
			std::getline(input, token, ' ');
			if (!token.empty()) {
				tokens.push_back(token);
			}
		}
		cmd = tokens[0];

		if (cmd == "setup") {
			std::string arg;
			std::for_each(
				tokens.begin() + 1,
				tokens.end(),
				[&arg](std::string token) {arg.append(token).append(" ");}
			);
			arg.pop_back(); // get rid of the extra space that was appended
			current_position.setup_position(arg);
		}
		else if (cmd == "display") {
			current_position.disp();
		}
		else if (cmd == "move") {

		}
		else if (cmd == "undo") {

		}
		else if (cmd == "perft") {
			std::string depth = tokens[1];
			Perft perft = Perft(current_position, std::stoi(depth));
			perft.run();
			std::cout << std::endl << perft << std::endl;
		}
		else if (cmd == "quit") {
			running = false;
		}
		else if (cmd == "help") {
			std::cout << std::endl;
			std::cout << "Usage:" << std::endl;
			std::cout << std::endl;
			std::cout << "setup <FEN>\t\t- creates a position based on the FEN string provided." << std::endl;
			std::cout << "display\t\t\t- shows the current position." << std::endl;
			std::cout << "move <LAN>...\t\t- makes a move. if multiple moves are provided, will make the moves in the sequence given. moves must be in long algebraic notation." << std::endl;
			std::cout << "undo [<count>]\t\t- reverses a number of moves. If count is omitted, reverses 1 move." << std::endl;
			std::cout << "perft <depth>\t\t- finds all moves to the depth given, and outputs aggregate information about the search." << std::endl;
			std::cout << "help\t\t\t- I think you already figured out what this does." << std::endl;
			std::cout << "quit\t\t\t- exit the program." << std::endl;
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
