#include "board/color.h"

#include <ostream>

namespace prodigy::board {
std::ostream& operator<<(std::ostream& os, const Color color) {
  os << (color == Color::WHITE ? 'w' : 'b');
  return os;
}
}
