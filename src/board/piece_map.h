#pragma once

#include <array>
#include <utility>

#include "board/color.h"
#include "board/piece_type.h"

namespace prodigy::board {
template <typename T>
class PieceMap : private std::array<T, 12> {
  using Base = std::array<T, 12>;

 public:
  template <Color COLOR, PieceType PIECE_TYPE>
  constexpr Base::reference get() {
    return (*this)[INDEX<COLOR, PIECE_TYPE>];
  }

  template <Color COLOR, PieceType PIECE_TYPE>
  constexpr Base::const_reference get() const {
    return (*this)[INDEX<COLOR, PIECE_TYPE>];
  }

 private:
  using Base::operator[];

  template <Color COLOR, PieceType PIECE_TYPE>
  static constexpr std::size_t INDEX = std::to_underlying(PIECE_TYPE) << 1 | std::to_underlying(COLOR);
};
}
