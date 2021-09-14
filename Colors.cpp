#include "Colors.h"
#include <iostream>
#include <string>

// operator overload allows Colors to be used with std::ostream object << operator
std::ostream& operator<<(std::ostream& out, Colors value) {
	std::string s;
	switch (value) {
		case(Colors::WHITE):	s = "White";	break;
		case(Colors::BLACK):	s = "Black";	break;
	}
	return out << s;
}

Colors operator!(Colors value) {
	return static_cast<Colors>(!bool(value));
}


