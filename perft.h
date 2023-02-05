#pragma once
#include <iostream>
#include "Position.h"
#include "perft.h"
#include "Bitboards.h"
#include <chrono>

// each array index is mapped to a square on the board and stores how many leaf nodes
// end on that particular square.
// 0 -> a1 ...
// 63 -> h8
typedef std::array<unsigned int, 64> position_counts;

class PerftCounts {
private:
	int moves;
	int captures;
	int en_passants;
	int castles;
	int promotions;
	int checks;
	int checkmates;
	int stalemates;
	int discoveries;
	int double_checks;
	position_counts positions;

public:
	PerftCounts() { this->clear(); };

	inline void move() { this->moves++; };
	inline void check() { this->checks++; };
	inline void capture() { this->captures++; };
	inline void ep() { this->en_passants++; };
	inline void castle() { this->castles++; };
	inline void promote() { this->promotions++; };
	inline void stalemate() { this->stalemates++; };
	inline void mate() { this->checkmates++; };
	inline void discover() { this->discoveries++; };
	inline void dblcheck() { this->double_checks++; };
	inline void add_position(std::string an) { this->positions[an2idx(an).value()]++; };
	inline void add_position(unsigned int idx) { this->positions[idx]++; };
	inline void add_position(Square sq) { add_position(sq.convert_to_index()); }
	inline void clear() {
		moves = 0;
		captures = 0;
		en_passants = 0;
		castles = 0;
		promotions = 0;
		checks = 0;
		checkmates = 0;
		stalemates = 0;
		discoveries = 0;
		double_checks = 0;
		positions.fill(0);
	}

	friend std::ostream& operator<<(std::ostream& out, PerftCounts counts);
};

//class SearchTreeNode {
//private:
//	Position position;
//	std::vector<SearchTreeNode*> children;
//public:
//
//	// array-like wrapper interface to the underlying array
//	inline SearchTreeNode& operator[](size_t pos) { return *children[pos]; };
//	inline SearchTreeNode& front() { return *children[0]; };
//	inline SearchTreeNode& back() { return *children.back(); };
//	inline bool empty() { return children.empty(); };
//	inline size_t size() { return children.size(); };
//
//	// vector-like wrapper interface to the underlying array
//	void push_back(SearchTreeNode node) {};
//};
//
//class SearchTree {
//private:
//	SearchTreeNode* root;
//	SearchTreeNode* current;
//public:
//	SearchTreeNode* get_nth_child
//};

class Perft {
private:
	PerftCounts counts;
	Position cur_pos;
	unsigned int max_depth;
	bool show_positions;
	double runtime;
	std::vector<Position> pos_history; // vector of past positions in the order they were played.
	std::vector<Move> move_history; // vector of the past moves in the order they were played.

	void perft_core(unsigned int depth);
public:
	Perft(int max_depth = 1, bool show_positions = false) :
		cur_pos(Position()),
		max_depth(max_depth),
		show_positions(show_positions)
	{
		counts = PerftCounts();
	};
	Perft(Position start_pos, int max_depth = 1, bool show_positions = false) :
		cur_pos(start_pos),
		max_depth(max_depth),
		show_positions(show_positions)
	{
		counts = PerftCounts();
	};
	Perft(std::string fen, int max_depth = 1, bool show_positions = false) :
		cur_pos(Position(fen)),
		max_depth(max_depth),
		show_positions(show_positions)
	{
		counts = PerftCounts();
	};

	void show_line(std::vector<Move> move_history, std::ostream& out);
	void run();
	void make_move(Move move);
	void undo_move();

	friend std::ostream& operator<<(std::ostream& out, Perft perft);
};



