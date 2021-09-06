#include "movegen/perft.h"

#include <ios>
#include <ostream>

#include "board/color.h"
#include "movegen/move_list.h"

namespace prodigy::movegen {
namespace {
template <board::Color ACTIVE_COLOR>
void perft(const MoveGenerator& move_generator, const board::Position& position,
           std::vector<std::uint64_t>& depth_to_node_count, const Ply depth = 0) {
  ++depth_to_node_count[depth];
  const auto moves = move_generator.generate<ACTIVE_COLOR>(position);
  if (depth == depth_to_node_count.size() - 2) {
    depth_to_node_count.back() += moves.size();
    return;
  }
  for (const auto move : moves) {
    perft<~ACTIVE_COLOR>(move_generator, position.apply<ACTIVE_COLOR>(move), depth_to_node_count, depth + 1);
  }
}
}

PerftResult perft(const MoveGenerator& move_generator, const board::Position& position, const Ply depth) {
  PerftResult result{};
  if (depth == 0) {
    result.depth_to_node_count.push_back(1);
    return result;
  }
  result.depth_to_node_count.resize(depth + 1);
  const auto start_time = std::chrono::steady_clock::now();
  position.active_color() == board::Color::WHITE
      ? perft<board::Color::WHITE>(move_generator, position, result.depth_to_node_count)
      : perft<board::Color::BLACK>(move_generator, position, result.depth_to_node_count);
  result.runtime = std::chrono::duration_cast<decltype(result.runtime)>(std::chrono::steady_clock::now() - start_time);
  return result;
}

std::ostream& operator<<(std::ostream& os, const PerftResult& result) {
  for (auto i = 0UZ; i < result.depth_to_node_count.size(); ++i) {
    os << " perft(" << i << "): " << result.depth_to_node_count[i] << '\n';
  }
  os << "      NPS: " << std::fixed << result.depth_to_node_count.back() * 1'000'000.0 / result.runtime.count() << '\n';
  os << "  Runtime: "
     << std::chrono::duration_cast<std::chrono::duration<double, std::chrono::seconds::period>>(result.runtime).count()
     << 's';
  return os;
}
}
