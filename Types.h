#pragma once
#include <iostream>
#include <string>
#include <algorithm>

enum Types { NONE = -1, PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5 };

std::ostream& operator<<(std::ostream& out, Types value);
std::istream& operator>>(std::istream& in, Types& value);

std::string	TypestoString(Types value);
char TypestoChar(Types value);
Types StringtoType(std::string str);
Types ChartoType(char c);