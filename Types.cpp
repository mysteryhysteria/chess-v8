#include "Types.h"

// operator overload allows Types to be used with std::ostream object << operator
std::ostream& operator<<(std::ostream& out, Types value) {
	return out << TypestoString(value);
}

std::istream& operator>>(std::istream& in, Types& value) {
	std::string str;
	if (!(in >> str)) { return in; };

	if (str.length() == 1) { value = ChartoType(str[0]); }
	else { value = StringtoType(str); };

	if (value == Types::NONE) {
		std::for_each(str.rbegin(), str.rend(), [&in](char c) {in.putback(c); });
		in.setstate(std::ios_base::failbit);
	}
	return in;
}

std::string TypestoString(Types value)
{
	std::string s;
	switch (value) {
	case(NONE):		return "None";
	case(PAWN):		return "Pawn";
	case(KNIGHT):	return "Knight";
	case(BISHOP):	return "Bishop";
	case(ROOK):		return "Rook";
	case(QUEEN):	return "Queen";
	case(KING):		return "King";
	}
}

char TypestoChar(Types value) {
	char c;
	switch (value) {
	case(PAWN):		return 'P';
	case(KNIGHT):	return 'N';
	case(BISHOP):	return 'B';
	case(ROOK):		return 'R';
	case(QUEEN):	return 'Q';
	case(KING):		return 'K';
	default:		return '~';
	}

}

Types StringtoType(std::string str) {
	// lowercase the string
	std::transform(str.begin(), str.end(), str.begin(),
		[](char c) { return std::tolower(c); });

	if		(str == "pawn"  )	{ return PAWN  ; }
	else if (str == "knight")	{ return KNIGHT; }
	else if (str == "bishop")	{ return BISHOP; }
	else if (str == "rook"  )	{ return ROOK  ; }
	else if (str == "queen" )	{ return QUEEN ; }
	else if (str == "king"  )	{ return KING  ; }
	else						{ return NONE  ; }
}

Types ChartoType(char c) {
	c = std::tolower(c); // lowercase the character

	switch (c) {
	case('p'): return PAWN;
	case('n'): return KNIGHT;
	case('b'): return BISHOP;
	case('r'): return ROOK;
	case('q'): return QUEEN;
	case('k'): return KING;
	default:   return NONE;
	}
}