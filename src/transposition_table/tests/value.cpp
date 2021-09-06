#define BOOST_TEST_MODULE Value

#include "transposition_table/value.h"

#include <bit>
#include <boost/test/tools/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>
#include <cstdint>
#include <type_traits>

#include "base/ply.h"
#include "board/coordinate.h"
#include "board/move.h"
#include "eval/score.h"
#include "search/node_type.h"

namespace prodigy::transposition_table {
namespace {
static_assert(std::is_trivially_copyable_v<Value>);

BOOST_AUTO_TEST_CASE(constructor) {
  static constexpr board::Move MOVE(board::Coordinate::E2, board::Coordinate::E4);
  static constexpr eval::Score SCORE = -25;
  static constexpr Ply DEPTH = 5;
  static constexpr auto NODE_TYPE = search::NodeType::PV;
  static constexpr Ply GENERATION = 40;
  static constexpr Value VALUE(MOVE, SCORE, DEPTH, NODE_TYPE, GENERATION);

  static_assert(VALUE.move() == MOVE);
  static_assert(VALUE.score() == SCORE);
  static_assert(VALUE.depth() == DEPTH);
  static_assert(VALUE.node_type() == NODE_TYPE);
  static_assert(VALUE.generation() == GENERATION);
  static_assert(VALUE.underlying() == std::bit_cast<std::uint64_t>(VALUE));
}

BOOST_AUTO_TEST_CASE(current_other_value_comparison) {
  static constexpr Ply CURRENT_GENERATION = 40;
  static constexpr Ply OTHER_DEPTH = 5;

#define _(NAME, DEPTH, GENERATION)                                                                   \
  static constexpr Value NAME(board::Move(board::Coordinate::E2, board::Coordinate::E4), -25, DEPTH, \
                              search::NodeType::PV, GENERATION)
  _(OTHER, OTHER_DEPTH, CURRENT_GENERATION);
  _(CURRENT_LESSER_DEPTH_VALUE, OTHER_DEPTH - 1, CURRENT_GENERATION);
  _(CURRENT_SAME_DEPTH_VALUE, OTHER_DEPTH, CURRENT_GENERATION);
  _(CURRENT_GREATER_DEPTH_VALUE, OTHER_DEPTH + 1, CURRENT_GENERATION);
  _(ANCIENT_LESSER_DEPTH_VALUE, OTHER_DEPTH - 1, CURRENT_GENERATION - 1);
  _(ANCIENT_SAME_DEPTH_VALUE, OTHER_DEPTH, CURRENT_GENERATION - 1);
  _(ANCIENT_GREATER_DEPTH_VALUE, OTHER_DEPTH + 1, CURRENT_GENERATION - 1);
#undef _

#define TEST(NAME, EXPECTED) static_assert(less_valuable_than(NAME, OTHER, CURRENT_GENERATION) == EXPECTED)
  TEST(CURRENT_LESSER_DEPTH_VALUE, true);
  TEST(CURRENT_SAME_DEPTH_VALUE, false);
  TEST(CURRENT_GREATER_DEPTH_VALUE, false);
  TEST(ANCIENT_LESSER_DEPTH_VALUE, true);
  TEST(ANCIENT_SAME_DEPTH_VALUE, true);
  TEST(ANCIENT_GREATER_DEPTH_VALUE, true);
#undef TEST
}

BOOST_AUTO_TEST_CASE(ancient_other_generation_comparison) {
  static constexpr Ply CURRENT_GENERATION = 40;
  static constexpr Ply ANCIENT_GENERATION = 39;
  static constexpr Ply OTHER_DEPTH = 5;

#define _(NAME, DEPTH, GENERATION)                                                                   \
  static constexpr Value NAME(board::Move(board::Coordinate::E2, board::Coordinate::E4), -25, DEPTH, \
                              search::NodeType::PV, GENERATION)
  _(OTHER, OTHER_DEPTH, ANCIENT_GENERATION);
  _(CURRENT_LESSER_DEPTH_VALUE, OTHER_DEPTH - 1, CURRENT_GENERATION);
  _(CURRENT_SAME_DEPTH_VALUE, OTHER_DEPTH, CURRENT_GENERATION);
  _(CURRENT_GREATER_DEPTH_VALUE, OTHER_DEPTH + 1, CURRENT_GENERATION);
  _(ANCIENT_LESSER_DEPTH_VALUE, OTHER_DEPTH - 1, ANCIENT_GENERATION);
  _(ANCIENT_SAME_DEPTH_VALUE, OTHER_DEPTH, ANCIENT_GENERATION);
  _(ANCIENT_GREATER_DEPTH_VALUE, OTHER_DEPTH + 1, ANCIENT_GENERATION);
  _(MORE_ANCIENT_LESSER_DEPTH_VALUE, OTHER_DEPTH - 1, ANCIENT_GENERATION - 1);
  _(MORE_ANCIENT_SAME_DEPTH_VALUE, OTHER_DEPTH, ANCIENT_GENERATION - 1);
  _(MORE_ANCIENT_GREATER_DEPTH_VALUE, OTHER_DEPTH + 1, ANCIENT_GENERATION - 1);
#undef _

#define TEST(NAME, EXPECTED) static_assert(less_valuable_than(NAME, OTHER, CURRENT_GENERATION) == EXPECTED)
  TEST(CURRENT_LESSER_DEPTH_VALUE, false);
  TEST(CURRENT_SAME_DEPTH_VALUE, false);
  TEST(CURRENT_GREATER_DEPTH_VALUE, false);
  TEST(ANCIENT_LESSER_DEPTH_VALUE, true);
  TEST(ANCIENT_SAME_DEPTH_VALUE, false);
  TEST(ANCIENT_GREATER_DEPTH_VALUE, false);
  TEST(MORE_ANCIENT_LESSER_DEPTH_VALUE, true);
  TEST(MORE_ANCIENT_SAME_DEPTH_VALUE, false);
  TEST(MORE_ANCIENT_GREATER_DEPTH_VALUE, false);
#undef TEST
}

BOOST_AUTO_TEST_CASE(output_stream) {
  boost::test_tools::output_test_stream os;
  os << Value(board::Move(board::Coordinate::E2, board::Coordinate::E4), -25, 5, search::NodeType::PV, 30);
  BOOST_TEST(
      os.is_equal("Value{ move=e2e4 score=-25 depth=5 node_type=PV generation=30 underlying=8444275069485852 }"));
}
}
}
