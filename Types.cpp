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
	case(Types::NONE):		return "None";
	case(Types::PAWN):		return "Pawn";
	case(Types::KNIGHT):	return "Knight";
	case(Types::BISHOP):	return "Bishop";
	case(Types::ROOK):		return "Rook";
	case(Types::QUEEN):	return "Queen";
	case(Types::KING):		return "King";
	}
}

char TypestoChar(Types value) {
	char c;
	switch (value) {
	case(Types::PAWN):		return 'P';
	case(Types::KNIGHT):	return 'N';
	case(Types::BISHOP):	return 'B';
	case(Types::ROOK):		return 'R';
	case(Types::QUEEN):	return 'Q';
	case(Types::KING):		return 'K';
	default:		return '~';
	}

}

Types StringtoType(std::string str) {
	// lowercase the string
	std::transform(str.begin(), str.end(), str.begin(),
		[](char c) { return std::tolower(c); });

	if		(str == "pawn"  )	{ return Types::PAWN  ; }
	else if (str == "knight")	{ return Types::KNIGHT; }
	else if (str == "bishop")	{ return Types::BISHOP; }
	else if (str == "rook"  )	{ return Types::ROOK  ; }
	else if (str == "queen" )	{ return Types::QUEEN ; }
	else if (str == "king"  )	{ return Types::KING  ; }
	else						{ return Types::NONE  ; }
}

Types ChartoType(char c) {
	c = std::tolower(c); // lowercase the character

	switch (c) {
	case('p'): return Types::PAWN;
	case('n'): return Types::KNIGHT;
	case('b'): return Types::BISHOP;
	case('r'): return Types::ROOK;
	case('q'): return Types::QUEEN;
	case('k'): return Types::KING;
	default:   return Types::NONE;
	}
}