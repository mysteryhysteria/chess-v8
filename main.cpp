// TODO: write a board display function, a bitboard display function, and a simple initialization script for the Position class to test FEN parsing.
#include "position.h"


int main() {
	Position P("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
	P.disp();
}