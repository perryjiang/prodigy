#pragma once

#include <bit>
#include <boost/assert.hpp>
#include <cstdint>
#include <iosfwd>
#include <string_view>
#include <utility>

namespace prodigy::board {
enum class CastlingRights : std::uint8_t {
  NONE,
  WHITE_KINGSIDE,
  WHITE_QUEENSIDE = 1 << 1,
  BLACK_KINGSIDE = 1 << 2,
  BLACK_QUEENSIDE = 1 << 3,
  ALL = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE,
};

constexpr CastlingRights operator~(const CastlingRights castling_rights) {
  return static_cast<CastlingRights>(~std::to_underlying(castling_rights));
}

constexpr CastlingRights operator&(const CastlingRights lhs, const CastlingRights rhs) {
  return static_cast<CastlingRights>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

constexpr CastlingRights operator|(const CastlingRights lhs, const CastlingRights rhs) {
  return static_cast<CastlingRights>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

constexpr CastlingRights& operator&=(CastlingRights& lhs, const CastlingRights rhs) { return lhs = lhs & rhs; }

constexpr CastlingRights& operator|=(CastlingRights& lhs, const CastlingRights rhs) { return lhs = lhs | rhs; }

constexpr CastlingRights to_castling_rights(const std::string_view castling_rights) {
  auto result = CastlingRights::NONE;
  for (const auto castling_right : castling_rights) {
    switch (castling_right) {
      case 'K':
        result |= CastlingRights::WHITE_KINGSIDE;
        break;
      case 'Q':
        result |= CastlingRights::WHITE_QUEENSIDE;
        break;
      case 'k':
        result |= CastlingRights::BLACK_KINGSIDE;
        break;
      case 'q':
        result |= CastlingRights::BLACK_QUEENSIDE;
        break;
    }
  }
  BOOST_ASSERT(castling_rights == "-" ||
               static_cast<std::size_t>(std::popcount(std::to_underlying(result))) == castling_rights.size());
  return result;
}

std::ostream& operator<<(std::ostream&, CastlingRights);
}
