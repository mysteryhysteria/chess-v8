#include "Bitboards.h"

// All bitboards use a 64-bit integer to represent the board.
// The LSB corresponds to A8 and the MSB corresponds to H1. 
// Therefore, the first byte is the 8th rank, the next byte is the 7th rank, etc.
// The first bit in each byte is in the A-file, the second bit in each byte is in the B-file, etc.
// 
// Examples:
// 
// These are all 64-bit integers (in hex) which only have a single '1' bit, meaning they represent single squares.
// 0x0000000000000001 -- A8
// 0x0000000000000002 -- B8
// 0x0000000000000004 -- C8
// 0x0000000000000008 -- D8
// 0x0000000000000010 -- E8
// 0x0000000000000020 -- F8
// 0x0000000000000040 -- G8
// 0x0000000000000080 -- H8
// 
// Each pair of hex digits represents one byte (and therefore one rank). 

std::array<uint64_t, 8> rank_masks = {
	0xff00000000000000, // 1st rank
	0x00ff000000000000, // 2nd rank
	0x0000ff0000000000, // 3rd rank
	0x000000ff00000000, // 4th rank
	0x00000000ff000000, // 5th rank
	0x0000000000ff0000, // 6th rank
	0x000000000000ff00, // 7th rank
	0x00000000000000ff  // 8th rank
};

std::array<uint64_t, 8> file_masks = {
	0x0101010101010101, // A file
	0x0202020202020202, // B file
	0x0404040404040404, // C file
	0x0808080808080808, // D file
	0x1010101010101010, // E file
	0x2020202020202020, // F file
	0x4040404040404040, // G file
	0x8080808080808080  // H file
};

// Bitboard Constructors
Bitboard::Bitboard(uint64_t bb) : bitboard(bb) {};
Bitboard::Bitboard(Square sq) : Bitboard(sq.get_u64()) {};
Bitboard::Bitboard() : Bitboard((uint64_t)0) {};

// Bitboard Methods
Bitboard& Bitboard::mark_square(Square sq) { return *this |= sq; };
Bitboard& Bitboard::clear_square(Square sq) { return *this &= ~sq; };
Bitboard& Bitboard::clear() { this->bitboard = (uint64_t)0; return *this; }
Bitboard& Bitboard::mirror_v() {
	this->bitboard =
		((bitboard & (uint64_t)0x00000000000000ff) << 56) |
		((bitboard & (uint64_t)0x000000000000ff00) << 40) |
		((bitboard & (uint64_t)0x0000000000ff0000) << 24) |
		((bitboard & (uint64_t)0x00000000ff000000) <<  8) |
		((bitboard & (uint64_t)0x000000ff00000000) >>  8) |
		((bitboard & (uint64_t)0x0000ff0000000000) >> 24) |
		((bitboard & (uint64_t)0x00ff000000000000) >> 40) |
		((bitboard & (uint64_t)0xff00000000000000) >> 56);

	return *this;
}
uint64_t Bitboard::get_u64() { return this->bitboard; };
bool Bitboard::is_empty() { return this->bitboard == (uint64_t)0; }
bool Bitboard::contains(Square sq) { return !(*this & sq).is_empty(); };

Square Bitboard::pop_occupied() {
	auto bb = this->bitboard;
	uint64_t next_sq = bb & (~bb + 1);
	this->bitboard &= (~next_sq);
	return Square(next_sq);
}

int Bitboard::popcount() { return std::bitset<64>(this->get_u64()).count(); };

// Bitboard Operator Overloads
Bitboard& Bitboard::operator|=(Square sq) { this->bitboard |= sq.get_u64(); return *this; };
Bitboard& Bitboard::operator&=(Square sq) { this->bitboard &= sq.get_u64(); return *this; };
Bitboard& Bitboard::operator|=(Bitboard bb) { this->bitboard |= bb.get_u64(); return *this; };
Bitboard& Bitboard::operator&=(Bitboard bb) { this->bitboard &= bb.get_u64(); return *this; };
Bitboard Bitboard::operator&(Square sq) { return Bitboard(this->bitboard & sq.get_u64()); };
Bitboard Bitboard::operator|(Square sq) { return Bitboard(this->bitboard | sq.get_u64()); };
Bitboard Bitboard::operator&(Bitboard bb) { return Bitboard(this->bitboard & bb.get_u64()); };
Bitboard Bitboard::operator|(Bitboard bb) { return Bitboard(this->bitboard | bb.get_u64()); };
Bitboard Bitboard::operator~() { return Bitboard(~(this->bitboard)); };

// Square Constructors
Square::Square(uint64_t sq) : bitboard(sq) { if (sq != 0) { ASSERT_ONE_SQUARE(*this); } };
Square::Square(Bitboard bb) : Square(bb.get_u64()) {};
Square::Square(int sq) : Square((uint64_t) sq) {};
Square::Square(unsigned int index) : Square((uint64_t)(1ULL << index)) {};
Square::Square(unsigned int rank, unsigned int file) : Square((unsigned int)((7 - rank) * 8 + file)) {};
Square::Square() : Square((uint64_t) 0) {};

// Square Operator overloads
Bitboard Square::operator&(const Square rhs) { return Bitboard(this->bitboard & rhs.bitboard); };
Bitboard Square::operator|(const Square rhs) { return Bitboard(this->bitboard | rhs.bitboard); };
Bitboard Square::operator&(Bitboard rhs) { return Bitboard(this->bitboard & rhs.get_u64()); };
Bitboard Square::operator|(Bitboard rhs) { return Bitboard(this->bitboard | rhs.get_u64()); };
Bitboard Square::operator~() { return Bitboard(~(this->bitboard)); };
Square Square::operator>>(const unsigned int n) { return Square(this->bitboard >> n); };
Square Square::operator<<(const unsigned int n) { return Square(this->bitboard << n); };
Square& Square::operator>>=(const unsigned int n) { this->bitboard >>= n; return *this; };
Square& Square::operator<<=(const unsigned int n) { this->bitboard <<= n; return *this; };
Square Square::operator+(const int n) {
	if		(n < 0) { return *this >> static_cast<unsigned int>(-n); }
	else if (n > 0) { return *this << static_cast<unsigned int>( n); }
	else			{ return *this; }
};

Square Square::operator-(const int n) {
	if		(n < 0) { return *this << static_cast<unsigned int>(-n); }
	else if (n > 0) { return *this >> static_cast<unsigned int>( n); }
	else			{ return *this; }
};

Square& Square::operator+=(const int n) { 
	if		(n < 0) { return *this >>= static_cast<unsigned int>(-n); }
	else if (n > 0) { return *this <<= static_cast<unsigned int>( n); }
	else			{ return *this; }
};

Square& Square::operator-=(const int n) {
	if		(n < 0) { return *this <<= static_cast<unsigned int>(-n); }
	else if (n > 0) { return *this >>= static_cast<unsigned int>( n); }
	else			{ return *this; }
};

Square Square::operator+(const Directions dir) { return *this + static_cast<int>(dir); };
Square Square::operator-(const Directions dir) { return *this - static_cast<int>(dir); };
Square& Square::operator+=(const Directions dir) { return *this += static_cast<int>(dir); };
Square& Square::operator-=(const Directions dir) { return *this -= static_cast<int>(dir); };
bool Square::operator!=(const Square rhs) { return bool(this->bitboard != rhs.bitboard); };
bool Square::operator!=(const uint64_t rhs) { return *this != Square(rhs); };
bool Square::operator==(const Square rhs) { return bool(this->bitboard == rhs.bitboard); };
bool Square::operator==(const uint64_t rhs) { return *this == Square(rhs); };

// Square Methods

bool Square::is_empty() { return this->bitboard == (uint64_t) 0; };

uint64_t Square::get_u64() { return this->bitboard; };

// TODO: rewrite this so that the function call isn't zero indexed.
bool Square::on_nth_rank(unsigned int n) {
	return bool(this->bitboard & rank_masks[n]);
}

// TODO: rewrite this so that the function call isn't zero indexed. Or make it take a char/string of the file.
bool Square::on_nth_file(unsigned int n) {
	return bool(this->bitboard & file_masks[n]);
}

bool Square::on_pawn_start_rank(Colors turn) {
	unsigned int n;
	!(turn) ? n = 1 : n = 6;
	return this->on_nth_rank(n);
}

bool Square::on_promote_rank(Colors turn) {
	if (turn == Colors::WHITE) {
		return this->on_nth_rank(7);
	}
	else if (turn == Colors::BLACK) {
		return this->on_nth_rank(0);
	}
	else {
		return false;
	}
}

unsigned int Square::convert_to_index() {
	ASSERT_ONE_SQUARE(*this);
	unsigned int index = 0;
	uint64_t bb = this->bitboard >> 1;
	while (bb != 0) { 
		bb = bb >> 1;
		++index; 
	}
	return index;
}

std::string Square::p2an() {
	return idx2an(this->convert_to_index()).value();
}

std::optional<Square> Square::next(unsigned int skip) {
	*this <<= skip;
	std::optional<Square> result = std::optional<Square>(std::nullopt);
	if (!this->is_empty()) {
		result = *this;
	}
	return result;
}

std::optional<Square> Square::previous(unsigned int skip) {
	*this >>= skip;
	std::optional<Square> result = std::optional<Square>(std::nullopt);
	if (!this->is_empty()) {
		result = *this;
	}
	return result;
}

Square& Square::first() { this->bitboard = (uint64_t) 1; return *this; }

int Square::popcount() { ASSERT_ONE_SQUARE(*this); return 1; }

int Square::direction_from(Square from)
{
	auto i_to = this->convert_to_index();
	auto i_from = from.convert_to_index();

	auto f_to = i_to % 8;
	auto r_to = (i_to - f_to) / 8;
	auto f_from = i_from % 8;
	auto r_from = (i_from - f_from) / 8;
	int df = f_to - f_from;
	int dr = r_to - r_from;
	
	if (df == 0 && dr == 0) { return 0; };

	int sm = fmin(abs(df), abs(dr));
	int lg = fmax(abs(df), abs(dr));

	if (sm == 0) {
		df = df / lg;
		dr = dr / lg;
	}
	else {
		if (lg % sm == 0) {
			df = df / sm; 
			dr = dr / sm;
		}
	}
	return dr * 8 + df;
}

bool Square::is_square(std::string an) {
	return this->get_u64() == (1ULL << (an2idx(an).value()));
}

uint64_t operator&=(uint64_t lhs, Square rhs) { return lhs &= (rhs.get_u64()); };
uint64_t operator|=(uint64_t lhs, Square rhs) { return lhs |= (rhs.get_u64()); };