#include <iostream>
#include "Position.h"
#include "perft.h"


std::ofstream write_perft_results(Perft perft, std::ofstream f) {
	f.open("C:/Users/Colin/source/repos/Chess Engine/tests/perft_results.txt", std::ios::out | std::ios::trunc);
	f.close();
}


std::ostream& PerftCounts::operator<<(std::ostream& out) {
	out << "Move count: " << this->moves << std::endl;
	out << "Capture count: " << this->captures << std::endl;
	out << "En Passant count: " << this->en_passants << std::endl;
	out << "Castles count: " << this->castles << std::endl;
	out << "Promotions count: " << this->promotions << std::endl;
	out << "Stalemates count: " << this->stalemates << std::endl;
	out << "Checks count: " << this->checks << std::endl;
	out << "Checkmates count: " << this->checkmates << std::endl;
	out << "Discovered Checks count: " << this->discoveries << std::endl;
	out << "Double Checks count: " << this->double_checks << std::endl;
	return out;
}

std::ostream& Perft::operator<<(std::ostream& out) {
	out << "Basic Perft, depth " << max_depth << ": " << std::endl;
	out << this->counts << std::endl;
	return out;
};

void Perft::perft() {
	perft_core(this->max_depth);
}

void Perft::perft_core(unsigned int depth) {
	Position position = pos_history.back();
	if (depth == 0) {
		counts.move();
		if (position.is_in_check()) {
			counts.check();
		}
		auto prev_move = move_history.back();
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
		}
		//disp_move_history(move_history, f);
	}
	else {
		int sum = 0;
		for (auto& move : position.BASIC_move_gen()) {
			position.make_move(move);
			position.check_integrity();

			move_history.push_back(move);
			pos_history.push_back(position);

			position.BASIC_king_threats();
			perft_core(depth - 1);
			undo_move();
		}
	}
}

void Perft::undo_move() {
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