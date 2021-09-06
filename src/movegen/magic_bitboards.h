#pragma once

#include <cstdint>
#include <functional>
#include <vector>

#include "board/bitboard.h"
#include "board/coordinate.h"
#include "board/coordinate_map.h"

namespace prodigy::movegen {
using Magic = std::uint64_t;
using AttackTable = std::vector<board::Bitboard>;

class MagicBitboards final {
 public:
  MagicBitboards(const std::function<bool(board::Coordinate origin, board::Coordinate target)>& mask_contains,
                 const std::function<board::Bitboard(board::Coordinate origin, board::Bitboard occupancy)>& attack_set);

  MagicBitboards(const MagicBitboards&) = delete;
  MagicBitboards& operator=(const MagicBitboards&) = delete;

  board::Bitboard attack_set(board::Coordinate origin, board::Bitboard occupancy) const;

 private:
  struct Record {
    board::Bitboard mask;
    Magic magic;
    std::uint8_t shift;
    AttackTable attack_table;
  };

  const board::CoordinateMap<Record> origin_to_record_;
};
}
