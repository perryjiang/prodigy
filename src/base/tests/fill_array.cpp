#define BOOST_TEST_MODULE FillArray

#include "base/fill_array.h"

#include <boost/test/unit_test.hpp>
#include <tuple>

namespace prodigy {
namespace {
BOOST_AUTO_TEST_CASE(construct_default) {
  for (const auto& [first, second] : fill_array<std::tuple<int, int>, 5>()) {
    BOOST_TEST(first == 0);
    BOOST_TEST(second == 0);
  }
}

BOOST_AUTO_TEST_CASE(construct_with_values) {
  for (const auto& [first, second, third] : fill_array<std::tuple<int, int, int>, 5>(1, 2, -5)) {
    BOOST_TEST(first == 1);
    BOOST_TEST(second == 2);
    BOOST_TEST(third == -5);
  }
}
}
}
