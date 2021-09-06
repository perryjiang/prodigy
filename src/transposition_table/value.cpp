#include "transposition_table/value.h"

#include <ostream>

namespace prodigy::transposition_table {
std::ostream& operator<<(std::ostream& os, const Value value) {
  os << "Value{ ";
  os << "move=" << value.move() << ' ';
  os << "score=" << value.score() << ' ';
  os << "depth=" << static_cast<std::size_t>(value.depth()) << ' ';
  os << "node_type=" << value.node_type() << ' ';
  os << "generation=" << static_cast<std::size_t>(value.generation()) << ' ';
  os << "underlying=" << value.underlying();
  os << " }";
  return os;
}
}
