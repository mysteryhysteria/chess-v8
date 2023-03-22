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
	case SpecialMoves::EN_PASSANT:
		return out << piece_type << '=' << from << " > " << to << " X " << capt_type << '=' << move.get_special().p2an();
	case SpecialMoves::CASTLE:
		if (move.get_to().on_nth_file(2)) {
			side = 'Q';
		}
		else if (move.get_to().on_nth_file(6)) {
			side = 'K';
		}
		return out << piece_type << '=' << from << " > " << to << " : " << side;
	case SpecialMoves::PROMOTION:
		return out << piece_type << '=' << from << " > " << to << " ^ " << promote_type;
	default:
		if (move.get_capt_type() == Types::NONE) {
			return out << piece_type << '=' << from << " > " << to;
		}
		else {
			return out << piece_type << '=' << from << " X " << capt_type << '=' << to;
		}
	}
}

Bitboard merge_moves(std::vector<Move> moves) {
	Bitboard all_moves;
	for (auto move : moves) {
		all_moves |= move.get_to();
	}
	return all_moves;
}