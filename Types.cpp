#include "Types.h"
#include <iostream>
#include <string>

// operator overload allows Types to be used with std::ostream object << operator
std::ostream& operator<<(std::ostream& out, Types value) {
	return out << TypestoString(value);
}

std::string TypestoString(Types value)
{
	std::string s;
	switch (value) {
	case(NONE):		s = "None";		break;
	case(PAWN):		s = "Pawn";		break;
	case(KNIGHT):	s = "Knight";	break;
	case(BISHOP):	s = "Bishop";	break;
	case(ROOK):		s = "Rook";		break;
	case(QUEEN):	s = "Queen";	break;
	case(KING):		s = "King";		break;
	}
	return s;
}

char TypestoChar(Types value) {
	char c;
	switch (value) {
	case(PAWN):		c = 'P';	break;
	case(KNIGHT):	c = 'N';	break;
	case(BISHOP):	c = 'B';	break;
	case(ROOK):		c = 'R';	break;
	case(QUEEN):	c = 'Q';	break;
	case(KING):		c = 'K';	break;
	default:		c = ' ';
	}
	return c;

}

