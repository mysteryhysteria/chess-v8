#include "Ray.h"

std::map<int, Bitboard> Ray::move_masks = { // map of move directions to bitmasks for the move generation
		{-17, Bitboard(~0x010101010101ffff)},
		{-15, Bitboard(~0x808080808080ffff)},
		{-10, Bitboard(~0x03030303030303ff)},
		{-9,  Bitboard(~0x01010101010101ff)},
		{-8,  Bitboard(~0x00000000000000ff)},
		{-7,  Bitboard(~0x80808080808080ff)},
		{-6,  Bitboard(~0xc0c0c0c0c0c0c0ff)},
		{-2,  Bitboard(~0x0303030303030303)},
		{-1,  Bitboard(~0x0101010101010101)},
		{1,   Bitboard(~0x8080808080808080)},
		{2,   Bitboard(~0xc0c0c0c0c0c0c0c0)},
		{6,   Bitboard(~0xff03030303030303)},
		{7,   Bitboard(~0xff01010101010101)},
		{8,   Bitboard(~0xff00000000000000)},
		{9,   Bitboard(~0xff80808080808080)},
		{10,  Bitboard(~0xffc0c0c0c0c0c0c0)},
		{15,  Bitboard(~0xffff010101010101)},
		{17,  Bitboard(~0xffff808080808080)}
};

Ray& Ray::operator++() {
	Bitboard next;
    if (!maxed) {
		if (direction > 0) { next = (this->current << direction) & move_masks[direction]; }
		else { next = (this->current >> abs(direction)) & move_masks[direction]; }
		if (!next.empty()) {
			--distance;
			this->current = next;
		}
		else {
			maxed = true;
		}
    }
    return *this;
}

Ray& Ray::operator--() {
    if (!maxed && current != base) {
		if (direction > 0) { this->current = (this->current >> direction) & move_masks[direction]; }
		else if (direction < 0) { this->current = (this->current << abs(direction)) & move_masks[direction]; }
		++distance;
	}
    return *this;
}

Square Ray::get_base() { return this->base; };

Square Ray::get_current() { return this->current; };

Ray& Ray::reset(int dir) {
    this->current = this->base;
    if (dir != 0) { this->direction = dir; };
    return *this;
}

bool Ray::end() { return maxed; };
