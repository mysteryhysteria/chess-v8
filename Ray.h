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
	Square get_next();
	Square get_prev();
	void update_maxed();

public:
	Ray(Square base, int direction, int max_distance = -1) : current(base), base(base), direction(direction), distance(max_distance) { update_maxed(); };
	Ray(Square base) : Ray(base, 0, 0) {};
	Ray() : Ray(Square(), 0, 0) {};

	static int NO_DIR;
	static int NO_MAX;

	Ray& operator++();
	Ray& operator--();

	Square get_base();
	Square get_current();
	Ray& reset(int dir = 0, int max_distance = -1);
	bool end();
};

