#pragma once

#include <chrono>
#include <cstdint>
#include <iosfwd>
#include <vector>

#include "base/ply.h"
#include "board/position.h"
#include "movegen/move_generator.h"

namespace prodigy::movegen {
struct PerftResult final {
  std::chrono::microseconds runtime;
  std::vector<std::uint64_t> depth_to_node_count;
};

PerftResult perft(const MoveGenerator&, const board::Position&, Ply depth);

std::ostream& operator<<(std::ostream&, const PerftResult&);
}
