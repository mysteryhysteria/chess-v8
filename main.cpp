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
	// Initial position
	//int depth = 5;
	//Perft perft = Perft(Position(), depth);
	//perft.run();
	//perft = Perft(Position().mirror_position(),depth);
	//perft.run();

	// Kiwipete
	// int depth = 4;
	// Perft perft = Perft(Position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"), depth);
	// perft.run();
	//perft = Perft(Position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1").mirror_position(), depth);
	//perft.run();

	Position current_position = Position();
	std::string token;
	bool running = true;

	// welcome message
	std::cout << "V8 by Colin Nordquist" << std::endl << std::endl;

	while (running) {
		std::cout << "v8 > " << std::endl;

		std::cin >> token;
		if (token == "setup") {
			std::string fen;
			std::cin >> fen;
			current_position = Position(fen);
		}
		else if (token == "display") {

		}
		else if (token == "move") {

		}
		else if (token == "undo") {

		}
		else if (token == "perft") {

		}
		else if (token == "quit") {

		}
		else {

		}
	}	
}
//	//Position P = Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); //Starting position
//	Position P = Position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"); //Kiwipete position
//	//Position P = Position("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"); // Position 3 from CPW
//	
//	// P.mirror_position();
//
//	P.disp();
//	perft_moves counts = { 0, 0, 0, 0, 0, 0 };
//
//	//int depth = 2;
//	for (int depth = 2; depth <= 2; ++depth) {
//		P.RUN_PERFT(depth, counts);
//	}
// }