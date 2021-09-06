#pragma once

#include <boost/assert.hpp>
#include <cstdint>
#include <iosfwd>

namespace prodigy::board {
enum class File : std::uint8_t {
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
};

constexpr File to_file(const char file) {
  BOOST_ASSERT('a' <= file && file <= 'h');
  return static_cast<File>(file - 'a');
}

std::ostream& operator<<(std::ostream&, File);
}
