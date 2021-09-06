#pragma once

#include <bit>
#include <boost/assert.hpp>
#include <concepts>
#include <cstdint>
#include <iosfwd>
#include <utility>

#include "board/coordinate.h"

namespace prodigy::board {
class Bitboard final {
 public:
  constexpr Bitboard() = default;

  constexpr explicit Bitboard(const Coordinate coordinate) : data_(1ULL << std::to_underlying(coordinate)) {}

  constexpr std::uint64_t underlying() const { return data_; }

  constexpr auto popcount() const { return std::popcount(data_); }

  constexpr Bitboard lsb() const { return Bitboard(data_ & -data_); }

  constexpr explicit operator bool() const { return data_; }

  constexpr Bitboard operator~() const { return Bitboard(~data_); }

  constexpr Bitboard& operator&=(const Bitboard rhs) {
    data_ &= rhs.data_;
    return *this;
  }

  constexpr Bitboard& operator|=(const Bitboard rhs) {
    data_ |= rhs.data_;
    return *this;
  }

  constexpr Bitboard& operator^=(const Bitboard rhs) {
    data_ ^= rhs.data_;
    return *this;
  }

  constexpr void pop_lsb() { data_ &= data_ - 1; }

 private:
  constexpr explicit Bitboard(const std::uint64_t data) : data_(data) {}

  std::uint64_t data_ = 0;
};

constexpr bool operator==(const Bitboard lhs, const Bitboard rhs) { return lhs.underlying() == rhs.underlying(); }

constexpr Bitboard operator&(Bitboard lhs, const Bitboard rhs) { return lhs &= rhs; }

constexpr Bitboard operator|(Bitboard lhs, const Bitboard rhs) { return lhs |= rhs; }

constexpr Bitboard operator^(Bitboard lhs, const Bitboard rhs) { return lhs ^= rhs; }

constexpr Coordinate unsafe_to_coordinate(const Bitboard bitboard) {
  BOOST_ASSERT(std::has_single_bit(bitboard.underlying()));
  return static_cast<Coordinate>(std::countr_zero(bitboard.underlying()));
}

template <std::invocable<Bitboard> Callback>
constexpr void for_each_bit(Bitboard bitboard, Callback&& callback) {
  while (bitboard) {
    std::forward<Callback>(callback)(bitboard.lsb());
    bitboard.pop_lsb();
  }
}

template <std::invocable<Coordinate> Callback>
constexpr void for_each_coordinate(const Bitboard bitboard, Callback&& callback) {
  for_each_bit(bitboard,
               [&](const auto bitboard) { std::forward<Callback>(callback)(unsafe_to_coordinate(bitboard)); });
}

std::ostream& operator<<(std::ostream&, Bitboard);
}
