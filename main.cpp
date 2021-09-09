// TODO: write a board display function, a bitboard display function, and a simple initialization script for the Position class to test FEN parsing.
#include "position.h"
#include "Colors.h"
#include "Move.h"
#include "Utils.h"

#include <map>
#include <string>

// stores a mapping from piece types to what move directions they have for their standard moves.

int main() {
	Position P("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
	P.disp();
	for (auto move : P.move_gen()) {
		std::cout << move;
	}
}

