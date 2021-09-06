#pragma once

#include <boost/assert.hpp>
#include <cstdint>
#include <optional>
#include <utility>

#include "board/coordinate.h"
#include "board/file.h"
#include "board/rank.h"

namespace prodigy::board {
enum class Direction : std::int8_t {
  NORTH = 8,
  EAST = 1,
  SOUTH = -NORTH,
  WEST = -EAST,
  NORTH_EAST = NORTH + EAST,
  SOUTH_EAST = SOUTH + EAST,
  SOUTH_WEST = SOUTH + WEST,
  NORTH_WEST = NORTH + WEST,
};

template <Direction DIRECTION>
constexpr std::optional<Coordinate> directional_offset(const Coordinate coordinate) {
  if constexpr (DIRECTION == Direction::NORTH) {
    if (rank_of(coordinate) == Rank::EIGHT) {
      return std::nullopt;
    }
  } else if constexpr (DIRECTION == Direction::EAST) {
    if (file_of(coordinate) == File::H) {
      return std::nullopt;
    }
  } else if constexpr (DIRECTION == Direction::SOUTH) {
    if (rank_of(coordinate) == Rank::ONE) {
      return std::nullopt;
    }
  } else if constexpr (DIRECTION == Direction::WEST) {
    if (file_of(coordinate) == File::A) {
      return std::nullopt;
    }
  } else if constexpr (DIRECTION == Direction::NORTH_EAST) {
    if (file_of(coordinate) == File::H || rank_of(coordinate) == Rank::EIGHT) {
      return std::nullopt;
    }
  } else if constexpr (DIRECTION == Direction::SOUTH_EAST) {
    if (file_of(coordinate) == File::H || rank_of(coordinate) == Rank::ONE) {
      return std::nullopt;
    }
  } else if constexpr (DIRECTION == Direction::SOUTH_WEST) {
    if (file_of(coordinate) == File::A || rank_of(coordinate) == Rank::ONE) {
      return std::nullopt;
    }
  } else {
    static_assert(DIRECTION == Direction::NORTH_WEST);
    if (file_of(coordinate) == File::A || rank_of(coordinate) == Rank::EIGHT) {
      return std::nullopt;
    }
  }
  return static_cast<Coordinate>(std::to_underlying(coordinate) + std::to_underlying(DIRECTION));
}

template <Direction DIRECTION>
constexpr Coordinate unsafe_directional_offset(const Coordinate coordinate) {
  const auto offset_coordinate =
      static_cast<Coordinate>(std::to_underlying(coordinate) + std::to_underlying(DIRECTION));
  BOOST_ASSERT(offset_coordinate == directional_offset<DIRECTION>(coordinate));
  return offset_coordinate;
}
}
