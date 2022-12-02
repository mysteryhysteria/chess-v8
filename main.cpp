#define BASIC

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

	//Position P = Position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); //Starting position
	Position P = Position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"); //Kiwipete position
	//Position P = Position("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"); // Position 3 from CPW
	
	P.disp();
	perft_moves counts = { 0, 0, 0, 0, 0, 0 };

	//int depth = 2;
	for (int depth = 1; depth <= 3; ++depth) {
		P.RUN_PERFT(depth, counts);
	}
}

