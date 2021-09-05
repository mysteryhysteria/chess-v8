#pragma once
#include <iostream>

enum Colors { WHITE = 0, BLACK = 1 };

std::ostream& operator<<(std::ostream& out, Colors value);
