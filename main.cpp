// TODO: write a board display function, a bitboard display function, and a simple initialization script for the Position class to test FEN parsing.

#include "Position.h"
#include "Colors.h"
#include "Move.h"
#include "Utils.h"

#include <map>
#include <string>
#include <iostream>

// stores a mapping from piece types to what move directions they have for their standard moves.

int main() {

	Position P = Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); //Starting position
	//Position P = Position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"); //Kiwipete position
	P.disp();
	perft_moves counts = { 0, 0, 0, 0, 0, 0 };

	//int depth = 2;
	for (int depth = 3; depth <= 5; ++depth) {
		P.perft(depth, counts);
		std::cout << "Perft, depth " << depth << ": " << std::endl;
		std::cout << "Move count: " << counts.moves << std::endl;
		std::cout << "Capture count: " << counts.capts << std::endl;
		std::cout << "En Passant count: " << counts.eps << std::endl;
		std::cout << "Castles count: " << counts.castles << std::endl;
		std::cout << "Promotions count: " << counts.promos << std::endl;
		std::cout << "Checks count: " << counts.checks << std::endl << std::endl;
		counts = { 0, 0, 0, 0, 0, 0 };
	}
}

