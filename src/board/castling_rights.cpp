#include "board/castling_rights.h"

#include <ostream>

namespace prodigy::board {
std::ostream& operator<<(std::ostream& os, const CastlingRights castling_rights) {
  if (castling_rights == CastlingRights::NONE) {
    os << '-';
    return os;
  }
  if ((castling_rights & CastlingRights::WHITE_KINGSIDE) != CastlingRights::NONE) {
    os << 'K';
  }
  if ((castling_rights & CastlingRights::WHITE_QUEENSIDE) != CastlingRights::NONE) {
    os << 'Q';
  }
  if ((castling_rights & CastlingRights::BLACK_KINGSIDE) != CastlingRights::NONE) {
    os << 'k';
  }
  if ((castling_rights & CastlingRights::BLACK_QUEENSIDE) != CastlingRights::NONE) {
    os << 'q';
  }
  return os;
}
}
