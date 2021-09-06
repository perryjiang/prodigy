#pragma once

#include <cstdint>
#include <iosfwd>

namespace prodigy::search {
enum class NodeType : std::uint8_t {
  PV,
  CUT,
  ALL,
};

std::ostream& operator<<(std::ostream&, NodeType);
}
