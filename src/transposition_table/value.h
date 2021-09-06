#pragma once

#include <bit>
#include <boost/assert.hpp>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <type_traits>
#include <utility>

#include "base/ply.h"
#include "board/move.h"
#include "eval/score.h"
#include "search/node_type.h"

namespace prodigy::transposition_table {
class alignas(std::uint64_t) Value final {
 public:
  constexpr Value(const board::Move move, const eval::Score score, const Ply depth, const search::NodeType node_type,
                  const Ply generation)
      : move_(move), score_(score), depth_(depth), node_type_(node_type), generation_(generation) {
    BOOST_ASSERT(std::in_range<decltype(score_)>(score));
  }

  constexpr board::Move move() const { return move_; }
  constexpr eval::Score score() const { return score_; }
  constexpr Ply depth() const { return depth_; }
  constexpr search::NodeType node_type() const { return node_type_; }
  constexpr Ply generation() const { return generation_; }
  constexpr std::uint64_t underlying() const { return std::bit_cast<std::uint64_t>(*this); }

  friend constexpr bool operator==(Value, Value) = default;

 private:
  board::Move move_;
  std::int16_t score_;
  Ply depth_;
  search::NodeType node_type_;
  Ply generation_;
  std::byte padding_{0};
};
static_assert(std::has_unique_object_representations_v<Value>);

constexpr bool less_valuable_than(const Value lhs, const Value rhs, const Ply generation) {
  return rhs.generation() == generation ? (lhs.generation() == generation ? lhs.depth() < rhs.depth() : true)
                                        : (lhs.generation() == generation ? false : lhs.depth() < rhs.depth());
}

std::ostream& operator<<(std::ostream&, Value);
}
