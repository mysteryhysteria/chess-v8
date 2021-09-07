#include <vector>

uint64_t merge_moves(std::vector<uint64_t> moves) {
	uint64_t all_moves = 0;
	for (auto move : moves) {
		all_moves |= move;
	}
	return all_moves;
}