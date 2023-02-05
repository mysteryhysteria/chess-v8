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

	Perft perft = Perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2);
	perft.run();

	//using namespace std;
	//string token;
	//bool quit = false;

	//// welcome message
	//cout << "V8 by Colin Nordquist" << endl;

	//while (!quit) {
	//	int depth;
	//	string filepath, fen;
	//	bool view, sort, no_summary;
	//	array<Types, 6> piece_filter;
	//	array<char, 2> square_filter;
	//	array<MoveTypes, 9> move_filter;
	//	array<MoveResults, 4> special;

	//	cout << "v8 > " << endl;
	//	
	//	cin >> token;
	//	if (token == "perft") {
	//		// default values
	//		fen = STARTING_POSITION;
	//		depth = 1;
	//		view = false;

	//		cin >> filepath;
	//		cin >> token;
	//		while (!cin.eof()) {
	//			if ((token == "--position" || token == "-p") && fen == STARTING_POSITION) {
	//				cin >> fen;
	//			}
	//			if ((token == "--depth" || token == "-d") && depth == 1) {
	//				cin >> depth;
	//			}
	//			if ((token == "--view" || token == "-v") && view == false) {
	//				view = true;
	//			}
	//			cin >> token;
	//		}
	//	}
	//	else if (token == "view") {
	//		// default values
	//		std::fill_n(piece_filter, piece_filter.size(), Types::NONE);
	//		square_filter = { 0, 0 };
	//		std::fill_n(move_filter, move_filter.size(), MoveTypes::NONE);
	//		std::fill_n(special, special.size(), MoveResults::NONE);
	//		sort = false;
	//		no_summary = false;

	//		cin >> filepath;
	//		cin >> token;
	//		while (!cin.eof()) {
	//			if ((token == "--filter-pieces" || token == "-fp") && piece_filter[0] == Types::NONE) {
	//				cin >> fen;
	//			}
	//			if ((token == "--filter-squares" || token == "-fs") && square_filter[0] == 0) {
	//				cin >> depth;
	//			}
	//			if ((token == "--filter-moves" || token == "-fm") && move_filter[0] == MoveTypes::NONE) {
	//				view = true;
	//			}
	//			if ((token == "--special" || token == "-sp") && fen == STARTING_POSITION) {
	//				cin >> fen;
	//			}
	//			if ((token == "--sort" || token == "-s") && fen == STARTING_POSITION) {
	//				cin >> fen;
	//			}
	//			if ((token == "--no-summary" || token == "-ns") && fen == STARTING_POSITION) {
	//				cin >> fen;
	//			}
	//			cin >> token;
	//		}
	//	}

	//	
	//	cout << "Stream state - " << endl;
	//	cout << "Good: " << cin.good() << endl;
	//	cout << "Fail: " << cin.fail() << endl;
	//	cout << "Bad: " << cin.bad() << endl;

	//	cout << perft_cmd;
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

