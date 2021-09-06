#define BOOST_TEST_MODULE Entry

#include "transposition_table/entry.h"

#include <boost/test/unit_test.hpp>

#include "board/coordinate.h"
#include "board/move.h"
#include "search/node_type.h"
#include "transposition_table/key.h"
#include "transposition_table/value.h"

namespace prodigy::transposition_table {
namespace {
BOOST_AUTO_TEST_CASE(entry) {
  static constexpr Key INITIAL_KEY = 100;
  static constexpr Value INITIAL_VALUE(board::Move(board::Coordinate::E2, board::Coordinate::E4), -25, 4,
                                       search::NodeType::PV, 15);
  static constexpr Key NEW_KEY = 100;
  static constexpr Value NEW_VALUE(board::Move(board::Coordinate::F2, board::Coordinate::F4), 12, 5,
                                   search::NodeType::ALL, 16);

  Entry entry(INITIAL_KEY, INITIAL_VALUE);
  const auto expect = [&](const auto key, const auto value) {
    const auto [stored_key, stored_value] = entry.load();
    BOOST_TEST(stored_key == key);
    BOOST_TEST(stored_value == value);
  };
  expect(INITIAL_KEY, INITIAL_VALUE);
  entry.store(NEW_KEY, NEW_VALUE);
  expect(NEW_KEY, NEW_VALUE);
}
}
}
