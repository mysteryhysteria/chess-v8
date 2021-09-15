#include "Bitboards.h"

std::array<uint64_t, 8> rank_masks = {
	0xff00000000000000,
	0x00ff000000000000,
	0x0000ff0000000000,
	0x000000ff00000000,
	0x00000000ff000000,
	0x0000000000ff0000,
	0x000000000000ff00,
	0x00000000000000ff
};

std::array<uint64_t, 8> file_masks = {
	0x0101010101010101,
	0x0202020202020202,
	0x0404040404040404,
	0x0808080808080808,
	0x1010101010101010,
	0x2020202020202020,
	0x4040404040404040,
	0x8080808080808080
};

// Bitboard Constructors
Bitboard::Bitboard(uint64_t bb) : bitboard(bb) {};
Bitboard::Bitboard(Square sq) : Bitboard(sq.get_bitboard()) {};
Bitboard::Bitboard() : Bitboard((uint64_t)0) {};

// Bitboard Methods
Bitboard& Bitboard::mark_square(Square& sq) { return *this |= sq; };
Bitboard& Bitboard::clear_square(Square& sq) { return *this |= sq; };
uint64_t Bitboard::get_bitboard() { return this->bitboard; };
bool Bitboard::empty() { return this->bitboard == (uint64_t)0; }
bool Bitboard::contains(Square& sq) { return !(*this & sq).empty(); };

Square Bitboard::pop_occupied() {
	auto bb = this->bitboard;
	auto next_sq = Square(bb & (~bb + 1));
	*this &= (~next_sq);
	return next_sq;
}

// Bitboard Operator Overloads
Bitboard& Bitboard::operator|=(Square& sq) { this->bitboard |= sq.get_bitboard(); return *this; };
Bitboard& Bitboard::operator&=(Square& sq) { this->bitboard &= sq.get_bitboard(); return *this; };
Bitboard& Bitboard::operator|=(Bitboard bb) { this->bitboard |= bb.get_bitboard(); return *this; };
Bitboard& Bitboard::operator&=(Bitboard bb) { this->bitboard &= bb.get_bitboard(); return *this; };
Bitboard Bitboard::operator&(Square& sq) { return Bitboard(this->bitboard & sq.get_bitboard()); };
Bitboard Bitboard::operator|(Square& sq) { return Bitboard(this->bitboard | sq.get_bitboard()); };
Bitboard Bitboard::operator&(Bitboard& bb) { return Bitboard(this->bitboard & bb.get_bitboard()); };
Bitboard Bitboard::operator|(Bitboard& bb) { return Bitboard(this->bitboard | bb.get_bitboard()); };

// Square Constructors
Square::Square(uint64_t sq) : bitboard(sq) { if (sq != 0) { ASSERT_ONE_SQUARE(*this); } };
Square::Square(Bitboard bb) : Square(bb.get_bitboard()) {};
Square::Square(int sq) : Square((uint64_t) sq) {};
Square::Square(unsigned int index) : Square((uint64_t)(1ULL << index)) {};
Square::Square(unsigned int rank, unsigned int file) : Square((unsigned int)((7 - rank) * 8 + file)) {};
Square::Square() : Square((uint64_t) 0) {};

// Square Operator overloads
Bitboard Square::operator&(const Square& rhs) { return Bitboard(this->bitboard & rhs.bitboard); };
Bitboard Square::operator|(const Square& rhs) { return Bitboard(this->bitboard | rhs.bitboard); };
Bitboard Square::operator&(Bitboard& rhs) { return Bitboard(this->bitboard & rhs.get_bitboard()); };
Bitboard Square::operator|(Bitboard& rhs) { return Bitboard(this->bitboard | rhs.get_bitboard()); };
Bitboard Square::operator~() { return Bitboard(~(this->bitboard)); };
Square& Square::operator>>=(const unsigned int n) { this->bitboard >>= n; return *this; };
Square& Square::operator<<=(const unsigned int n) { this->bitboard <<= n; return *this; };
Square& Square::operator+=(const unsigned int n) { return *this <<= n; };
Square& Square::operator-=(const unsigned int n) { return *this >>= n; };
Square Square::operator>>(const unsigned int n) { return Square(this->bitboard >> n); };
Square Square::operator<<(const unsigned int n) { return Square(this->bitboard << n); };
bool Square::operator!=(const Square& rhs) { return bool(this->bitboard != rhs.bitboard); };
bool Square::operator!=(const uint64_t& rhs) { return *this != Square(rhs); };
bool Square::operator==(const Square& rhs) { return bool(this->bitboard == rhs.bitboard); };
bool Square::operator==(const uint64_t& rhs) { return *this == Square(rhs); };

// Square Methods

bool Square::empty() { return this->bitboard == (uint64_t) 0; };

uint64_t Square::get_bitboard() { return this->bitboard; };

bool Square::on_nth_rank(unsigned int n) {
	return bool(this->bitboard & rank_masks[n]);
}

bool Square::on_nth_file(unsigned int n) {
	return bool(this->bitboard & file_masks[n]);
}

bool Square::on_pawn_start_rank(Colors turn) {
	unsigned int n;
	!(turn) ? n = 1 : n = 6;
	return this->on_nth_rank(n);
}

bool Square::on_promote_rank(Colors turn) {
	unsigned int n;
	!(turn) ? n = 7 : n = 0;
	return this->on_nth_rank(n);
}

unsigned int Square::convert_to_index() {
	unsigned int maxbit = 0;
	while ((*this >> (maxbit + 1)) != 0) maxbit++;
	return maxbit;
}

std::string Square::p2an() {
	std::string an;
	int sq = this->convert_to_index();
	auto file = sq % 8;
	auto rank = 7 - ((sq - file) / 8);

	return an.append(1, 'a' + file).append(1, '1' + rank);
}

Square& Square::next(unsigned int skip) {
	return *this <<= skip;
}

Square& Square::previous(unsigned int skip) {
	return *this >>= skip;
}

Square& Square::first() { this->bitboard = (uint64_t) 1; return *this; }

uint64_t operator&=(uint64_t lhs, Square rhs) { return lhs &= (rhs.get_bitboard()); };
uint64_t operator|=(uint64_t lhs, Square rhs) { return lhs |= (rhs.get_bitboard()); };