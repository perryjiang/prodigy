#include "board/piece_type.h"

#include <ostream>

namespace prodigy::board {
std::ostream& operator<<(std::ostream& os, const PieceType piece_type) {
  switch (piece_type) {
    case PieceType::PAWN:
      os << 'p';
      break;
    case PieceType::KNIGHT:
      os << 'n';
      break;
    case PieceType::BISHOP:
      os << 'b';
      break;
    case PieceType::ROOK:
      os << 'r';
      break;
    case PieceType::QUEEN:
      os << 'q';
      break;
    case PieceType::KING:
      os << 'k';
      break;
  }
  return os;
}
}
