#define BOOST_TEST_MODULE TranspositionTable

#include "transposition_table/transposition_table.h"

#include <boost/test/unit_test.hpp>
#include <optional>
#include <ostream>

#include "base/uniform_distribution.h"
#include "board/coordinate.h"
#include "board/move.h"
#include "search/node_type.h"
#include "transposition_table/key.h"
#include "transposition_table/value.h"

namespace prodigy::transposition_table {
std::ostream& boost_test_print_type(std::ostream& os, const std::optional<Value>& opt) {
  if (opt.has_value()) {
    os << *opt;
  } else {
    os << "nullopt";
  }
  return os;
}

namespace {
BOOST_AUTO_TEST_CASE(advance_generation) {
  const auto make_value = [](const auto generation) {
    return Value(board::Move(board::Coordinate::E2, board::Coordinate::E4), 5, 3, search::NodeType::PV, generation);
  };

  TranspositionTable transposition_table;
  const auto key = uniform_distribution<Key>();
  const auto initial_value = make_value(0);
  BOOST_TEST(!transposition_table.find(key).has_value());
  BOOST_TEST(transposition_table.try_insert(key, initial_value.move(), initial_value.score(), initial_value.depth(),
                                            initial_value.node_type()));
  BOOST_TEST(transposition_table.find(key) == make_value(0));
  transposition_table.advance_generation();
  BOOST_TEST(transposition_table.find(key) == make_value(1));
  BOOST_TEST(!transposition_table.find(key + 1).has_value());
}
}
}
