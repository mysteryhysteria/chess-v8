#include "Ray.h"

bool operator==(Directions lhs, int rhs) {
	return static_cast<int>(lhs) == rhs;
}

bool operator==(int lhs, Directions rhs) {
	return lhs == static_cast<int>(rhs);
}

bool operator==(Directions lhs, Directions rhs) {
	return static_cast<int>(lhs) == static_cast<int>(rhs);
}

Square Ray::get_next() {
	if (direction > 0) {
		return ((this->current) << direction);
	}
	else if (direction < 0) {
		return ((this->current) >> (-direction));
	}
	else { // direction == 0
		return Square();
	}
}

Square Ray::get_prev() {
	if (direction > 0) {
		return ((this->current) >> direction); // no need to check against the move mask as we must have already found this square
	}
	else if (direction < 0) {
		return ((this->current) << (-direction));
	}
	else { // direction == 0
		return Square();
	}
}

void Ray::update_maxed() { this->maxed = (distance == 0) || (this->current & move_masks.at(static_cast<Directions>(direction))).is_empty(); }

Ray& Ray::operator++() {
	if (!maxed) {
		--distance;
		this->current = get_next();
		update_maxed();
	}
	return *this;
}

Ray& Ray::operator--() {
	if (this->current != this->base) {
		++distance;
		this->current = get_prev();
	}	
	return *this;
}

Square Ray::get_base() { return this->base; };

Square Ray::get_current() { return this->current; };

Ray& Ray::reset(Directions dir, int max_distance) {
    this->current = this->base;
	this->distance = max_distance;
	this->direction = static_cast<int>(dir);
	update_maxed();
    return *this;
}

bool Ray::end() { return maxed; };
