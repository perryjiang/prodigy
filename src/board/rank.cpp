#include "board/rank.h"

#include <ostream>
#include <utility>

namespace prodigy::board {
std::ostream& operator<<(std::ostream& os, const Rank rank) {
  os << static_cast<char>('1' + std::to_underlying(rank));
  return os;
}
}
