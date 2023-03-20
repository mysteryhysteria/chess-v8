#include "perft.h"

std::ostream& operator<<(std::ostream& out, PerftCounts counts) {
	out << "Move count: " << counts.moves << std::endl;
	out << "Capture count: " << counts.captures << std::endl;
	out << "En Passant count: " << counts.en_passants << std::endl;
	out << "Castles count: " << counts.castles << std::endl;
	out << "Promotions count: " << counts.promotions << std::endl;
	out << "Stalemates count: " << counts.stalemates << std::endl;
	out << "Checks count: " << counts.checks << std::endl;
	out << "Checkmates count: " << counts.checkmates << std::endl;
	out << "Discovered Checks count: " << counts.discoveries << std::endl;
	out << "Double Checks count: " << counts.double_checks << std::endl;
	out << "Final Position counts:" << std::endl;

	//for (int i = 0; i < counts.end_positions.size(); i++) {
	//	if (counts.end_positions[i] != 0) {
	//		out << "\t" << idx2an(i).value() << ": " << counts.end_positions[i] << std::endl;
	//	}
	//}

	out << "Starting Move counts:" << std::endl;
	for (auto start_move : counts.start_moves) {
		out << "\t" << start_move.first << ": " << start_move.second << std::endl;
	}

	return out;
}

std::ostream& operator<<(std::ostream& out, Perft perft) {
	out << "Basic Perft, depth " << perft.max_depth << ": " << std::endl;
	out << perft.counts << std::endl;
	out << std::fixed << std::setprecision(3)
		<< "Total Runtime: " << perft.runtime << "s" << std::endl;
	return out;
};

void Perft::run() {
	auto t_start = std::chrono::high_resolution_clock::now();
	perft_core(this->max_depth);
	auto t_end = std::chrono::high_resolution_clock::now();

	std::chrono::duration<double> diff = t_end - t_start;
	runtime = diff.count();

	std::cout << *this << std::endl;
}

inline void PerftCounts::add_start_move(std::string start_move_SAN) {
	if (start_moves.contains(start_move_SAN)) {
		start_moves[start_move_SAN]++;
	}
	else {
		start_moves[start_move_SAN] = 1;
	}
}

void Perft::perft_core(unsigned int depth) {
	if (depth == 0) {
		counts.move();
		if (cur_pos.is_in_check()) {
			counts.check();
		}
		auto prev_move = move_history.back();
		auto first_move = move_history.front();
		switch (prev_move.get_move_type()) {
		case STD:
			if (prev_move.get_capt_type() != NONE) {
				counts.capture();
			}
			break;
		case EN_PASSANT:
			counts.capture();
			counts.ep();
			break;
		case PROMOTION:
			counts.promote();
			break;
		case CASTLE:
			counts.castle();
			break;
		default:
			break;
		}
		counts.add_end_position(prev_move.get_to());
		std::string start_move = "";
		counts.add_start_move(start_move + first_move.get_from().p2an() + first_move.get_to().p2an());
	}
	else {
		for (auto& move : cur_pos.BASIC_move_gen()) {

			make_move(move);
			if (!cur_pos.is_position_illegal()) {
				if (show_positions) {
					std::cout << "Perft depth: " << max_depth - depth + 1 << std::endl;
					std::cout << "Current Move: " << move << std::endl;
					std::cout << "Current Position: " << std::endl;
					cur_pos.disp();
				}
				perft_core(depth - 1);
			}
			undo_move();
		}
	}
}

void Perft::make_move(Move move) {
	move_history.push_back(move);
	pos_history.push_back(cur_pos);

	cur_pos.make_move(move);
	cur_pos.check_integrity();
	cur_pos.BASIC_king_threats();
}

void Perft::undo_move() {
	cur_pos = pos_history.back();
	pos_history.pop_back();
	move_history.pop_back();
}

void Perft::show_line(std::vector<Move> move_history, std::ostream& out) {
	auto move_it = move_history.begin();
	while (move_it != move_history.end()) {
		out << *move_it;
		if (++move_it == move_history.end()) {
			out << std::endl;
		}
		else {
			out << ", ";
		}
	}
};
