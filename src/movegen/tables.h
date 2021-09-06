#pragma once

#include "board/bitboard.h"
#include "board/color.h"
#include "board/coordinate.h"
#include "board/coordinate_map.h"
#include "board/piece_type.h"
#include "movegen/magic_bitboards.h"

namespace prodigy::movegen {
class Tables final {
 public:
  Tables();

  Tables(const Tables&) = delete;
  Tables& operator=(const Tables&) = delete;

  template <board::Color, board::PieceType>
  board::Bitboard attack_set(board::Coordinate origin, board::Bitboard occupancy) const;
  board::Bitboard ray(board::Coordinate origin, board::Coordinate target) const;

 private:
  const board::CoordinateMap<board::Bitboard> white_pawn_attack_table_;
  const board::CoordinateMap<board::Bitboard> black_pawn_attack_table_;
  const board::CoordinateMap<board::Bitboard> knight_attack_table_;
  const board::CoordinateMap<board::Bitboard> king_attack_table_;
  const MagicBitboards bishop_magic_bitboards_;
  const MagicBitboards rook_magic_bitboards_;
  const board::CoordinateMap<board::CoordinateMap<board::Bitboard>> ray_table_;
};

template <board::Color COLOR, board::PieceType PIECE_TYPE>
board::Bitboard Tables::attack_set(const board::Coordinate origin, const board::Bitboard occupancy) const {
  if constexpr (PIECE_TYPE == board::PieceType::PAWN) {
    if constexpr (COLOR == board::Color::WHITE) {
      return white_pawn_attack_table_[origin];
    } else {
      static_assert(COLOR == board::Color::BLACK);
      return black_pawn_attack_table_[origin];
    }
  } else if constexpr (PIECE_TYPE == board::PieceType::KNIGHT) {
    return knight_attack_table_[origin];
  } else if constexpr (PIECE_TYPE == board::PieceType::KING) {
    return king_attack_table_[origin];
  } else if constexpr (PIECE_TYPE == board::PieceType::BISHOP) {
    return bishop_magic_bitboards_.attack_set(origin, occupancy);
  } else if constexpr (PIECE_TYPE == board::PieceType::ROOK) {
    return rook_magic_bitboards_.attack_set(origin, occupancy);
  } else {
    static_assert(PIECE_TYPE == board::PieceType::QUEEN);
    return bishop_magic_bitboards_.attack_set(origin, occupancy) | rook_magic_bitboards_.attack_set(origin, occupancy);
  }
}
}
