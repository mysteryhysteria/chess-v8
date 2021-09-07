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
	auto all_moves = merge_moves(P.move_gen_generic(1ULL << 39, { -9, -8, -7, -1, 1, 7, 8, 9}, 1));
	P.disp_bitboard(all_moves, "All Moves");
}