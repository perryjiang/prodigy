#include "search/random_searcher.h"

#include "base/uniform_distribution.h"
#include "board/color.h"
#include "movegen/move_list.h"

namespace prodigy::search {
std::optional<board::Move> RandomSearcher::search(const board::Position& position) {
  if (const auto moves = position.active_color() == board::Color::WHITE
                             ? move_generator_.generate<board::Color::WHITE>(position)
                             : move_generator_.generate<board::Color::BLACK>(position);
      !moves.empty()) {
    return moves[uniform_distribution<decltype(moves)::size_type>(0, moves.size() - 1)];
  }
  return std::nullopt;
}
}
