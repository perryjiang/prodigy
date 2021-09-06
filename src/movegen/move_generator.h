#pragma once

#include "board/bitboard.h"
#include "board/color.h"
#include "board/coordinate.h"
#include "board/piece_type.h"
#include "board/position.h"
#include "movegen/move_list.h"
#include "movegen/tables.h"

namespace prodigy::movegen {
class MoveGenerator final {
 public:
  MoveGenerator() = default;

  MoveGenerator(const MoveGenerator&) = delete;
  MoveGenerator& operator=(const MoveGenerator&) = delete;

  template <board::Color ACTIVE_COLOR>
  MoveList generate(const board::Position&) const;

 private:
  template <board::Color, board::PieceType>
  board::Bitboard pseudo_legal_move_set(const board::Position&, board::Coordinate origin,
                                        board::Bitboard occupancy) const;

  const Tables tables_;
};
}
