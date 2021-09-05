#include "Types.h"
#include <iostream>
#include <string>

// operator overload allows Types to be used with std::ostream object << operator
std::ostream& operator<<(std::ostream& out, Types value) {
	std::string s;
	switch (value) {
		case(PAWN):		s = "Pawn";		break;
		case(KNIGHT):	s = "Knight";	break;
		case(BISHOP):	s = "Bishop";	break;
		case(ROOK):		s = "Rook";		break;
		case(QUEEN):	s = "Queen";	break;
		case(KING):		s = "King";		break;
	}
	return out << s;
}
