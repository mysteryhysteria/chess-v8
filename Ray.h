#pragma once
#include "Bitboards.h"

// TODO: Reimplement the Ray to use iterators (more idiomatic).

const int NO_MAX = -1;

class Ray {
private:
	Square current;
	Square base;
	int direction;
	int distance;
	bool maxed;
	Square get_next();
	Square get_prev();
	void update_maxed();

public:
	Ray(Square base, Directions direction, int max_distance = -1) : current(base), base(base), direction(static_cast<int>(direction)), distance(max_distance) { update_maxed(); };
	Ray(Square base) : Ray(base, Directions::NO_DIRECTION, 0) {};
	Ray() : Ray(Square(), Directions::NO_DIRECTION, 0) {};

	Ray& operator++();
	Ray& operator--();

	Square get_base();
	Square get_current();
	Ray& reset(Directions dir = Directions::NO_DIRECTION, int max_distance = -1);
	bool end();
};

