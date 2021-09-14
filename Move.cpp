#include "Move.h"

std::ostream& operator<<(std::ostream& out, Move move)
{
	SpecialMoves move_type = move.get_move_type();
	char	piece_type = TypestoChar(move.get_type()),
			capt_type = TypestoChar(move.get_capt_type()),
			promote_type = TypestoChar(move.get_promote_type()),
			side = ' ';
	std::string		from = move.get_from().p2an(),
					to = move.get_to().p2an();

	switch (move_type) {
	case EN_PASSANT:
		return std::cout << piece_type << '=' << from << " > " << to << " X " << capt_type << '=' << move.get_special().p2an() << std::endl;
	case CASTLE:
		if (move.get_to().on_nth_file(2)) {
			side = 'Q';
		}
		else if (move.get_to().on_nth_file(6)) {
			side = 'K';
		}
		return std::cout << piece_type << '=' << from << " > " << to << " : " << side << std::endl;
	case PROMOTION:
		return std::cout << piece_type << '=' << from << " > " << to << " ^ " << promote_type << std::endl;
	default:
		if (move.get_capt_type() == NONE) {
			return std::cout << piece_type << '=' << from << " > " << to << std::endl;
		}
		else {
			return std::cout << piece_type << '=' << from << " X " << capt_type << '=' << to << std::endl;
		}
	}
}

uint64_t merge_moves(std::vector<Move> moves) {
	uint64_t all_moves = 0ULL;
	for (auto move : moves) {
		all_moves |= move.get_to();
	}
	return all_moves;
}