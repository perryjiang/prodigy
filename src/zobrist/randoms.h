#pragma once

#include "base/enum_map.h"
#include "board/castling_rights.h"
#include "board/color.h"
#include "board/coordinate.h"
#include "board/coordinate_map.h"
#include "board/file.h"
#include "board/piece_map.h"
#include "board/piece_type.h"
#include "zobrist/hash.h"

namespace prodigy::zobrist {
class Randoms final {
 public:
  static const Randoms& instance();

  Randoms(const Randoms&) = delete;
  Randoms& operator=(const Randoms&) = delete;

  template <board::Color, board::PieceType>
  Hash coordinate_random(board::Coordinate) const;
  Hash active_color_random() const;
  Hash castling_rights_random(board::CastlingRights) const;
  Hash en_passant_file_random(board::File) const;

 private:
  Randoms();

  const board::PieceMap<board::CoordinateMap<Hash>> piece_to_coordinate_to_random_;
  const Hash active_color_random_;
  const EnumMap<board::CastlingRights, Hash, 16> castling_rights_to_random_;
  const EnumMap<board::File, Hash, 8> en_passant_file_to_random_;
};

template <board::Color COLOR, board::PieceType PIECE_TYPE>
Hash Randoms::coordinate_random(const board::Coordinate coordinate) const {
  return piece_to_coordinate_to_random_.get<COLOR, PIECE_TYPE>()[coordinate];
}
}
