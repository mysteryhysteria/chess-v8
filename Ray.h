#pragma once
#include "Bitboards.h"

class Ray {
private:
	static std::map<int, Bitboard> move_masks;

	Square current;
	Square base;
	int direction;
	int distance;
	bool maxed;

public:
	Ray(Square base, int direction, int max_distance = -1) : current(base), base(base), direction(direction), distance(max_distance), maxed(max_distance == 0) {};
	Ray() : Ray(Square(), 1, 0) {};

	Ray& operator++();
	Ray& operator--();

	Square get_base();
	Square get_current();
	Ray& reset(int dir = 0);
	Square pop();
	bool end();
};

