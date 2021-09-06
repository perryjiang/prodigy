#pragma once

#include <concepts>
#include <cstdint>
#include <iosfwd>
#include <utility>

#include "board/file.h"
#include "board/rank.h"

namespace prodigy::board {
enum class Coordinate : std::uint8_t {
  A1,
  B1,
  C1,
  D1,
  E1,
  F1,
  G1,
  H1,
  A2,
  B2,
  C2,
  D2,
  E2,
  F2,
  G2,
  H2,
  A3,
  B3,
  C3,
  D3,
  E3,
  F3,
  G3,
  H3,
  A4,
  B4,
  C4,
  D4,
  E4,
  F4,
  G4,
  H4,
  A5,
  B5,
  C5,
  D5,
  E5,
  F5,
  G5,
  H5,
  A6,
  B6,
  C6,
  D6,
  E6,
  F6,
  G6,
  H6,
  A7,
  B7,
  C7,
  D7,
  E7,
  F7,
  G7,
  H7,
  A8,
  B8,
  C8,
  D8,
  E8,
  F8,
  G8,
  H8,
};

constexpr Coordinate to_coordinate(const File file, const Rank rank) {
  return static_cast<Coordinate>(std::to_underlying(rank) * 8 + std::to_underlying(file));
}

constexpr File file_of(const Coordinate coordinate) { return static_cast<File>(std::to_underlying(coordinate) % 8); }

constexpr Rank rank_of(const Coordinate coordinate) { return static_cast<Rank>(std::to_underlying(coordinate) / 8); }

template <std::invocable<Coordinate> Callback>
constexpr void for_each_coordinate(Callback&& callback) {
  for (auto coordinate = Coordinate::A1; coordinate <= Coordinate::H8;
       coordinate = static_cast<Coordinate>(std::to_underlying(coordinate) + 1)) {
    std::forward<Callback>(callback)(coordinate);
  }
}

std::ostream& operator<<(std::ostream&, Coordinate);
}
