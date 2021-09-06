#pragma once

#include <boost/assert.hpp>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string_view>
#include <utility>

#include "board/coordinate.h"
#include "board/piece_type.h"

namespace prodigy::board {
class Move final {
 public:
  constexpr Move(const Coordinate origin, const Coordinate target,
                 const std::optional<PieceType> promotion = std::nullopt)
      : data_((promotion.has_value() ? std::to_underlying(*promotion) : 0) << 12 | std::to_underlying(origin) << 6 |
              std::to_underlying(target)) {
    BOOST_ASSERT(!promotion.has_value() || (*promotion != PieceType::PAWN && *promotion != PieceType::KING));
  }

  constexpr explicit Move(const std::string_view move)
      : Move(to_coordinate(to_file(move[0]), to_rank(move[1])), to_coordinate(to_file(move[2]), to_rank(move[3])),
             move.size() == 5 ? std::optional(to_piece_type(move.back())) : std::nullopt) {
    BOOST_ASSERT(move.size() == 4 || move.size() == 5);
  }

  constexpr Coordinate origin() const { return static_cast<Coordinate>(data_ >> 6 & 0x3F); }

  constexpr Coordinate target() const { return static_cast<Coordinate>(data_ & 0x3F); }

  constexpr std::optional<PieceType> promotion() const {
    return data_ >> 12 ? std::optional(static_cast<PieceType>(data_ >> 12)) : std::nullopt;
  }

  friend constexpr auto operator<=>(Move, Move) = default;

 private:
  std::uint16_t data_;
};

std::ostream& operator<<(std::ostream&, Move);
}
