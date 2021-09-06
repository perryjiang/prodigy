#include "board/coordinate.h"

#include <ostream>

namespace prodigy::board {
std::ostream& operator<<(std::ostream& os, const Coordinate coordinate) {
  os << file_of(coordinate) << rank_of(coordinate);
  return os;
}
}
