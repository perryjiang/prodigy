#include "board/bitboard.h"

#include <iomanip>
#include <ios>
#include <ostream>

namespace prodigy::board {
namespace {
class HexWriter final {
 public:
  explicit HexWriter(std::ostream& os) : os_(os), flags_(os.flags()) {}

  ~HexWriter() { os_.flags(flags_); }

  void write(const Bitboard bitboard) {
    os_ << "0x" << std::hex << std::setfill('0') << std::setw(16) << bitboard.underlying();
  }

 private:
  std::ostream& os_;
  const std::ios_base::fmtflags flags_;
};
}

std::ostream& operator<<(std::ostream& os, const Bitboard bitboard) {
  HexWriter(os).write(bitboard);
  return os;
}
}
