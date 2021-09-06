#include "board/move.h"

#include <ostream>

namespace prodigy::board {
std::ostream& operator<<(std::ostream& os, const Move move) {
  os << move.origin() << move.target();
  if (const auto promotion = move.promotion(); promotion.has_value()) {
    os << *promotion;
  }
  return os;
}
}
