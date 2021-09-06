#pragma once

#include <boost/assert.hpp>
#include <cstdint>
#include <iosfwd>
#include <utility>

namespace prodigy::board {
enum class Color : std::uint8_t {
  WHITE,
  BLACK,
};

constexpr Color to_color(const char color) {
  BOOST_ASSERT(color == 'w' || color == 'b');
  return color == 'w' ? Color::WHITE : Color::BLACK;
}

constexpr Color operator~(const Color color) { return static_cast<Color>(std::to_underlying(color) ^ 1); }

std::ostream& operator<<(std::ostream&, Color);
}
