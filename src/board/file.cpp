#include "board/file.h"

#include <ostream>
#include <utility>

namespace prodigy::board {
std::ostream& operator<<(std::ostream& os, const File file) {
  os << static_cast<char>('a' + std::to_underlying(file));
  return os;
}
}
