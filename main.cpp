// TODO: write a board display function, a bitboard display function, and a simple initialization script for the Position class to test FEN parsing.
#include "Position.h"
#include "Colors.h"
#include "Move.h"
#include "Utils.h"

#include <map>
#include <string>

// stores a mapping from piece types to what move directions they have for their standard moves.

int main() {
	Position P("r3k2r/8/8/8/3q4/8/3B4/R3K2R w KQkq - 0 1");
	P.disp();
	for (auto move : P.move_gen()) {
		std::cout << move;
	}
}

