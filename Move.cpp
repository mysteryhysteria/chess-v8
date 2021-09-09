#include "Move.h"
#include "SpecialMoves.h"
#include "Utils.h"
#include <assert.h>
#include <iostream>

std::ostream& operator<<(std::ostream& out, Move move)
{
	SpecialMoves move_type = move.get_move_type();
	char	piece_type = TypestoChar(move.get_type()),
			capt_type = TypestoChar(move.get_capt_type()),
			promote_type = TypestoChar(move.get_promote_type()),
			side = ' ';
	std::string		from = p2an(move.get_from()),
					to = p2an(move.get_to());

	switch (move_type) {
	case EN_PASSANT:
		return std::cout << piece_type << '=' << from << " > " << to << " X " << capt_type << '=' << p2an(move.get_special()) << std::endl;
	case CASTLE:
		if (on_nth_file(move.get_to(), 2)) {
			side = 'Q';
		}
		else if (on_nth_file(move.get_to(), 6)) {
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

//P=a4 > a5
//N=b7 X P=a5
//P=a4 > b5 X P=b4
//K=e1 > g1 : K
//P=b7 > b8 ^ Q